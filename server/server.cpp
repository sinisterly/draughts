#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>
#include <netinet/tcp.h>

constexpr int MSG_SIZE=20;

struct threadData
{
	int fd;
	int playerId=0;
	int gameId=0;
	std::string nick;
};

struct Player{
	int fd=-1;
	std::string nick="Empty";
	int place=0;
	int id=0;
	std::queue<std::string> commands;
};

struct Place{
	int playerId=0;
	bool ready=false;
};

struct Game{
	std::vector <Player> players;
	Place places[2];
	bool started=false;
	//std::string place1="Empty";
	//std::string place2="Empty";
	Game(){
		players.push_back(Player());
	}
	int addPlayer(int fd,std::string &nick){
		Player p;
		p.fd=fd;
		p.nick=nick;
		p.id=players.size();
		players.push_back(p);
		return p.id;
	}
	void addMessage(std::string msg,int fd,bool toMe=false){
		for(auto &p:players){
			if(p.id==0 && msg.substr(0,4)!="ruch") continue;
			if(p.fd!=fd || toMe)
				p.commands.push(msg);
		}
	}
	std::string getMessage(int playerId){
		if(players[playerId].commands.empty()) return "";
		std::string msg=players[playerId].commands.front();
		players[playerId].commands.pop();
		return msg;
	}
	
} games[200];

void errorInfo(const char *source){
	perror(source);
	exit(-1);
}

std::string readMessage(threadData *data){
	char buf[MSG_SIZE];
	int readBytes=0;
	Game &game=games[data->gameId];
	while(readBytes<MSG_SIZE)
	{
		int cnt=read(data->fd,buf,MSG_SIZE-readBytes);
		std::cout << "przeczytano: " << cnt << std::endl;
		if(cnt==0)
		{
			int place=game.players[data->playerId].place;
			std::cout << "miejsce " << place << std::endl;
			if(place!=0)
			{
				game.places[place-1]=Place();
				game.addMessage("leave"+std::to_string(place)+'\n',data->fd);
				if(game.started==true)
				{
					game.addMessage("end "+std::to_string(place==1 ? 0 : 2)+'\n',data->fd);
					game.players[0].commands=std::queue<std::string>();
				}
			}
			close(data->fd);
			std::cout << "rozlaczono klienta" << std::endl;
			data->fd=-1;
			pthread_exit(NULL);
		}
		else if(cnt==-1)
		{
			perror("nie udalo sie odczytac danych: ");
		}
		readBytes+=cnt;
	}
	return std::string(buf);
}

void sendMessage(int fd,std::string msg)
{
	int size=msg.size();
	int sentBytes=0;
	while(sentBytes<size)
	{
		int cnt=write(fd,msg.substr(sentBytes).c_str(),size-sentBytes);
		if(cnt==-1)
		{
			perror("nie udalo sie wyslac danych: ");
		}
		sentBytes+=cnt;
	}
}

void *readingThread(void *t_data)
{
	pthread_detach(pthread_self());
	threadData *data=(threadData*)t_data;
	Game &game=games[data->gameId];
	while(1){
		std::string msg=readMessage(data);
		//dodac assert msg.size()==20
		msg+='\n';
		std::cout << "info: " << msg << std::endl;
		if(msg.substr(0,4)=="sit1" && game.places[0].playerId==0)
		{
			std::cout << "usiadl1" << std::endl;
			game.places[0].playerId=data->playerId;
			game.players[data->playerId].place=1;
			game.players[data->playerId].commands.push("sit1ok\n");
			game.addMessage(msg,data->fd);
			if(game.places[1].playerId!=0)
			{
				game.players[game.places[0].playerId].commands.push("enableStart\n");
				game.players[game.places[1].playerId].commands.push("enableStart\n");
			}
		}
		if(msg.substr(0,4)=="sit2" && game.places[1].playerId==0)
		{
			std::cout << "usiadl2" << std::endl;
			game.places[1].playerId=data->playerId;
			game.players[data->playerId].place=2;
			game.players[data->playerId].commands.push("sit2ok\n");
			game.addMessage(msg,data->fd);
			if(game.places[0].playerId!=0)
			{
				game.players[game.places[0].playerId].commands.push("enableStart\n");
				game.players[game.places[1].playerId].commands.push("enableStart\n");
			}
		}
		if(msg.substr(0,6)=="leave1")
		{
			if(game.places[1].playerId!=0)
			{
				game.players[game.places[0].playerId].commands.push("disableStart\n");
				game.players[game.places[1].playerId].commands.push("disableStart\n");
			}
			game.places[0].playerId=0;
			game.places[0].ready=false;
			game.addMessage(msg,data->fd);
		}
		if(msg.substr(0,6)=="leave2")
		{
			if(game.places[0].playerId!=0)
			{
				game.players[game.places[0].playerId].commands.push("disableStart\n");
				game.players[game.places[1].playerId].commands.push("disableStart\n");
			}
			game.places[1].playerId=0;
			game.places[1].ready=false;
			game.addMessage(msg,data->fd);
		}
		if(msg.substr(0,4)=="ruch")
		{
			game.addMessage(msg,data->fd);
		}
		if(msg.substr(0,5)=="start")
		{
			game.places[game.players[data->playerId].place-1].ready=true;
			std::cout << "ready" << std::endl; 
			if(game.places[0].ready && game.places[1].ready)
			{
				game.addMessage("start\n",data->fd,true);
				game.started=true;
				game.places[0].ready=false;
				game.places[1].ready=false;
			}
		}
		if(msg.substr(0,9)=="drawOffer")
		{
			int opponentId=game.places[0].playerId==data->playerId ? game.places[1].playerId : game.places[0].playerId;
			game.players[opponentId].commands.push("drawOffer\n");
		}
		if(msg.substr(0,7)=="drawYes")
		{
			game.started=false;
			game.players[0].commands=std::queue<std::string>();
			game.addMessage("end 1\n",data->fd,true);
		}
		if(msg.substr(0,6)=="resign")
		{
			game.started=false;
			game.players[0].commands=std::queue<std::string>();
			int place1Score=game.places[0].playerId==data->playerId ? 0 : 2;
			game.addMessage("end "+std::to_string(place1Score)+'\n',data->fd,true);
		}
		if(msg.substr(0,3)=="win")
		{
			game.started=false;
			game.players[0].commands=std::queue<std::string>();
			int place1Score=game.places[0].playerId==data->playerId ? 2 : 0;
			game.addMessage("end "+std::to_string(place1Score)+'\n',data->fd,true);
		}
			
		
		//std::cout << "Wiadomosc: " << msg << std::endl;
		//game.addMessage(msg,data->fd);
	}
	
}

void *writingThread(void *t_data)
{
	pthread_detach(pthread_self());
	threadData *data=(threadData*)t_data;
	std::cout << "gameId = " << data->gameId << " fd = " << data->fd << " playerId = " << data->playerId << std::endl;
	
	int id=games[data->gameId].places[0].playerId;
	std::string msg="sit1 " + games[data->gameId].players[id].nick + '\n';
	sendMessage(data->fd,msg);
	
	id=games[data->gameId].places[1].playerId;
	msg="sit2 " + games[data->gameId].players[id].nick + '\n';
	sendMessage(data->fd,msg);
	
	auto q=games[data->gameId].players[0].commands;
	while(!q.empty()){
		std::string msg=q.front();
		q.pop();
		sendMessage(data->fd,msg);
	}
	while(data->fd!=-1){
		std::string msg=games[data->gameId].getMessage(data->playerId);
		if(msg=="") continue;
		std::cout << "wysylamy wiadomosc: " << msg << std::endl;
		sendMessage(data->fd,msg);
	}
	std::cout << "zamykamy watek wysylajacy" << std::endl;
	pthread_exit(NULL);
}

void handleConnection(int userFd)
{
	threadData *data=new threadData();

	data->fd=userFd;
	data->gameId=std::stoi(readMessage(data));
	data->nick=readMessage(data);

	//std::cout << "table: " << data->gameId << std::endl;
	data->playerId=games[data->gameId].addPlayer(userFd,data->nick);
	pthread_t thread1;
	int rTh=pthread_create(&thread1,NULL,readingThread,data);
	if(rTh) errorInfo("blad przy tworzeniu watku readingThread: ");
	
	pthread_t thread2;
	int wTh=pthread_create(&thread2,NULL,writingThread,data);
	if(wTh) errorInfo("blad przy tworzeniu watku writingThread: ");
	
}

int main(){
	struct sockaddr_in sa;
	memset(&sa,0,sizeof(sa));
	sa.sin_family=AF_INET;
	sa.sin_port=htons(1050);
	sa.sin_addr.s_addr=htonl(INADDR_ANY);
	int serverFd=socket(AF_INET,SOCK_STREAM,0);
	if(serverFd<0) errorInfo("Blad przy wywolaniu funkcji socket: ");
	int nFoo=1;
	setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, (char*)&nFoo, sizeof(nFoo));

	if(bind(serverFd,(struct sockaddr*)&sa,sizeof(sa))<0) 
		errorInfo("Blad przy wywolaniu funkcji bind: ");
	if(listen(serverFd,100)<0)
		errorInfo("Blad przy wywolaniu funkcji listen: ");
		
	
	
	while(1)
	{
		int userFd=accept(serverFd,NULL,NULL);
		if(userFd<0) errorInfo("Blad przy wywolaniu funkcji accept: ");
		
		/*int enableKeepAlive = 1;
		setsockopt(userFd, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive, sizeof(enableKeepAlive));

		int maxIdle = 10; // seconds 
		setsockopt(userFd, IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));

		int count = 3;  // send up to 3 keepalive packets out, then disconnect if no response
		setsockopt(userFd, SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));

		int interval = 2;   // send a keepalive packet out every 2 seconds (after the 5 second idle period)
		setsockopt(userFd, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));*/
		
		std::cout << userFd << std::endl;
		handleConnection(userFd);
		
	}
	
	close(serverFd);
	return 0;
}
