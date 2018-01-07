#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

constexpr int MSG_SIZE = 20;

struct threadData {
  int fd;
  int playerId = 0;
  int gameId = 0;
  std::string nick;
};

struct Player {
  int fd = -1;
  std::string nick = "Empty";
  int place = 0;
  int id = 0;
  std::queue<std::string> commands;
  pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
  void addMessage(const std::string &msg) {
    pthread_mutex_lock(&queueMutex);
    commands.push(msg);
    pthread_mutex_unlock(&queueMutex);
  }
};

struct Place {
  int playerId = 0;
  bool ready = false;
};

struct Game {
  std::vector<Player> players;
  Place places[2];
  bool started = false;
  Game() { players.push_back(Player()); }
  int addPlayer(int fd, const std::string &nick) {
    Player p;
    p.fd = fd;
    p.nick = nick;
    p.id = players.size();
    players.push_back(p);
    return p.id;
  }
  void addMessage(const std::string &msg, int fd, bool toMe = false) {
    for (auto &p : players) {
      if (p.id == 0 && msg.substr(0, 4) != "ruch") continue;
      if (p.fd != fd || toMe) {
        p.addMessage(msg);
      }
    }
  }
  std::string getMessage(int playerId) {
    pthread_mutex_lock(&players[playerId].queueMutex);
    if (players[playerId].commands.empty()) {
      pthread_mutex_unlock(&players[playerId].queueMutex);
      return "";
    }
    std::string msg = players[playerId].commands.front();
    players[playerId].commands.pop();
    pthread_mutex_unlock(&players[playerId].queueMutex);
    return msg;
  }

} games[200];

void errorInfo(const char *source) {
  perror(source);
  exit(-1);
}

std::string readMessage(threadData *data) {
  char buf[MSG_SIZE];
  int readBytes = 0;
  Game &game = games[data->gameId];
  std::string msg = "";
  while (readBytes < MSG_SIZE) {
    int cnt = read(data->fd, buf, MSG_SIZE - readBytes);
    if (cnt > 0) {
      msg += std::string(buf, cnt);
      readBytes += cnt;
    } else if (cnt == 0)  // klient się rozłączył, należy poddać grę jeśli był
                          // graczem i zwolnić jego zasoby(wątki i deskryptor)
    {
      int place = game.players[data->playerId].place;
      if (place != 0) {
        game.places[place - 1] = Place();
        if (game.started == true) {
          game.started = false;
          game.addMessage("end " + std::to_string(place == 1 ? 0 : 2) + '\n',
                          data->fd);
          pthread_mutex_lock(&game.players[0].queueMutex);
          game.players[0].commands = std::queue<std::string>();
          pthread_mutex_unlock(&game.players[0].queueMutex);
        }
        game.addMessage("leave" + std::to_string(place) + '\n', data->fd);
        game.players[game.places[0].playerId].addMessage("disableStart\n");
        game.players[game.places[1].playerId].addMessage("disableStart\n");
      }
      close(data->fd);
      std::cout << "rozlaczono klienta" << std::endl;
      std::cout << "zamykamy watek czytajacy gracza o fd = " << data->fd
                << std::endl;
      data->fd = -1;
      pthread_exit(NULL);
    } else if (cnt == -1) {
      perror("nie udalo sie odczytac danych: ");
    }
  }
  std::cout << "odczytano wiadomosc od gracza o fd = " << data->fd << ": "
            << msg << std::endl;
  return msg;
}

void sendMessage(int fd, const std::string &msg) {
  int size = msg.size();
  int sentBytes = 0;
  while (sentBytes < size) {
    int cnt = write(fd, msg.substr(sentBytes).c_str(), size - sentBytes);
    if (cnt == -1) {
      perror("nie udalo sie wyslac danych: ");
    }
    sentBytes += cnt;
  }
  std::cout << "wyslano wiadomosc do gracza o fd = " << fd << ": " << msg
            << std::endl;
}

void *readingThread(void *t_data) {
  pthread_detach(pthread_self());
  threadData *data = (threadData *)t_data;
  Game &game = games[data->gameId];
  while (1) {
    std::string msg = readMessage(data);
    msg += '\n';
    if (msg.substr(0, 4) == "sit1" && game.places[0].playerId == 0) {
      game.places[0].playerId = data->playerId;
      game.players[data->playerId].place = 1;
      game.players[data->playerId].addMessage("sit1ok\n");
      game.addMessage(msg, data->fd);
      if (game.places[1].playerId != 0) {
        game.players[game.places[0].playerId].addMessage("enableStart\n");
        game.players[game.places[1].playerId].addMessage("enableStart\n");
      }
    }
    if (msg.substr(0, 4) == "sit2" && game.places[1].playerId == 0) {
      game.places[1].playerId = data->playerId;
      game.players[data->playerId].place = 2;
      game.players[data->playerId].addMessage("sit2ok\n");
      game.addMessage(msg, data->fd);
      if (game.places[0].playerId != 0) {
        game.players[game.places[0].playerId].addMessage("enableStart\n");
        game.players[game.places[1].playerId].addMessage("enableStart\n");
      }
    }
    if (msg.substr(0, 6) == "leave1" && game.started == false) {
      if (game.places[1].playerId != 0) {
        game.players[game.places[0].playerId].addMessage("disableStart\n");
        game.players[game.places[1].playerId].addMessage("disableStart\n");
      }
      game.places[0].playerId = 0;
      game.places[0].ready = false;
      game.addMessage(msg, data->fd, true);
    }
    if (msg.substr(0, 6) == "leave2" && game.started == false) {
      if (game.places[0].playerId != 0) {
        game.players[game.places[0].playerId].addMessage("disableStart\n");
        game.players[game.places[1].playerId].addMessage("disableStart\n");
      }
      game.places[1].playerId = 0;
      game.places[1].ready = false;
      game.addMessage(msg, data->fd, true);
    }
    if (msg.substr(0, 4) == "ruch") {
      game.addMessage(msg, data->fd);
    }
    if (msg.substr(0, 5) == "start") {
      game.places[game.players[data->playerId].place - 1].ready = true;
      if (game.places[0].ready && game.places[1].ready) {
        game.addMessage("start\n", data->fd, true);
        game.started = true;
        game.places[0].ready = false;
        game.places[1].ready = false;
      }
    }
    if (msg.substr(0, 9) == "drawOffer") {
      int opponentId = game.places[0].playerId == data->playerId
                           ? game.places[1].playerId
                           : game.places[0].playerId;
      game.players[opponentId].addMessage("drawOffer\n");
    }
    if (msg.substr(0, 7) == "drawYes") {
      game.started = false;
      pthread_mutex_lock(&game.players[0].queueMutex);
      game.players[0].commands = std::queue<std::string>();
      pthread_mutex_unlock(&game.players[0].queueMutex);
      game.addMessage("end 1\n", data->fd, true);
    }
    if (msg.substr(0, 6) == "resign") {
      game.started = false;
      pthread_mutex_lock(&game.players[0].queueMutex);
      game.players[0].commands = std::queue<std::string>();
      pthread_mutex_unlock(&game.players[0].queueMutex);
      int place1Score = game.places[0].playerId == data->playerId ? 0 : 2;
      game.addMessage("end " + std::to_string(place1Score) + '\n', data->fd,
                      true);
    }
    if (msg.substr(0, 3) == "win") {
      game.started = false;
      pthread_mutex_lock(&game.players[0].queueMutex);
      game.players[0].commands = std::queue<std::string>();
      pthread_mutex_unlock(&game.players[0].queueMutex);
      int place1Score = game.places[0].playerId == data->playerId ? 2 : 0;
      game.addMessage("end " + std::to_string(place1Score) + '\n', data->fd,
                      true);
    }
  }
}

void *writingThread(void *t_data) {
  pthread_detach(pthread_self());
  threadData *data = (threadData *)t_data;
  std::cout << "gameId = " << data->gameId << " fd = " << data->fd
            << " playerId = " << data->playerId << std::endl;
  Game &game = games[data->gameId];
  int id = game.places[0].playerId;
  std::string msg = "sit1 " + game.players[id].nick + '\n';
  sendMessage(data->fd, msg);

  id = game.places[1].playerId;
  msg = "sit2 " + game.players[id].nick + '\n';
  sendMessage(data->fd, msg);

  pthread_mutex_lock(&game.players[0].queueMutex);
  auto q = game.players[0].commands;
  pthread_mutex_unlock(&game.players[0].queueMutex);
  while (!q.empty()) {
    std::string msg = q.front();
    q.pop();
    sendMessage(data->fd, msg);
  }
  int fd = data->fd;
  while (data->fd != -1) {
    std::string msg = game.getMessage(data->playerId);
    if (msg == "") continue;
    sendMessage(data->fd, msg);
  }
  std::cout << "zamykamy watek wysylajacy gracza o fd = " << fd << std::endl;
  pthread_exit(NULL);
}

void handleConnection(int userFd) {
  threadData *data = new threadData();

  data->fd = userFd;
  data->gameId = std::stoi(readMessage(data));
  data->nick = readMessage(data);

  data->playerId = games[data->gameId].addPlayer(userFd, data->nick);
  pthread_t thread1;
  int rTh = pthread_create(&thread1, NULL, readingThread, data);
  if (rTh) errorInfo("blad przy tworzeniu watku readingThread: ");

  pthread_t thread2;
  int wTh = pthread_create(&thread2, NULL, writingThread, data);
  if (wTh) errorInfo("blad przy tworzeniu watku writingThread: ");
}

int main() {
  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(1050);
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  int serverFd = socket(AF_INET, SOCK_STREAM, 0);
  if (serverFd < 0) errorInfo("Blad przy wywolaniu funkcji socket: ");
  int nFoo = 1;
  setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, (char *)&nFoo, sizeof(nFoo));

  if (bind(serverFd, (struct sockaddr *)&sa, sizeof(sa)) < 0)
    errorInfo("Blad przy wywolaniu funkcji bind: ");
  if (listen(serverFd, 100) < 0)
    errorInfo("Blad przy wywolaniu funkcji listen: ");

  while (1) {
    int userFd = accept(serverFd, NULL, NULL);
    if (userFd < 0) errorInfo("Blad przy wywolaniu funkcji accept: ");

    // poniższe ustawienia pozwolają wykryć stracone połączenie klienta z
    // internetem np. po odłączeniu kabla po 10 sekundach zamiast domyślnych na
    // systemach Linux 2 godzinach
    int enableKeepAlive = 1;
    setsockopt(userFd, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive,
               sizeof(enableKeepAlive));
    int maxIdle = 10;  // seconds
    setsockopt(userFd, IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));
    int count = 3;  // send up to 3 keepalive packets out, then disconnect if no
                    // response
    setsockopt(userFd, SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));
    int interval = 2;  // send a keepalive packet out every 2 seconds (after the
                       // 5 second idle period)
    setsockopt(userFd, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));

    handleConnection(userFd);
  }

  close(serverFd);
  return 0;
}
