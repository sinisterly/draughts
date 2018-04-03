#include "connection.h"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <QMessageBox>
#include <sstream>
#include "ui_connection.h"

Connection::Connection(QWidget* parent)
    : QDialog(parent),
      ui(new Ui::Connection),
      tcpSocket(new QTcpSocket(this)),
      w(new MainWindow()) {
  ui->setupUi(this);
  ui->connectButton->setEnabled(false);
  ui->table->setValidator(new QIntValidator(0, 199, this));
  ui->port->setValidator(new QIntValidator(0, 65535, this));
  ui->nick->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9]*"), this));
  w->getGame()->setConnection(this);
  connect(ui->nick, &QLineEdit::textChanged, this,
          &Connection::enableConnectButton);
  connect(ui->table, &QLineEdit::textChanged, this,
          &Connection::enableConnectButton);
  connect(tcpSocket, &QIODevice::readyRead, this, &Connection::readData);
  connect(tcpSocket,
          QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
          this, &Connection::displayError);
  connect(tcpSocket, &QAbstractSocket::connected, this, &Connection::connected);
  connect(tcpSocket, &QAbstractSocket::disconnected, this,
          &Connection::disconnected);
}

Connection::~Connection() { delete ui; }

void Connection::sendMove(const std::string& msg) { sendMessage(msg); }

QTcpSocket* Connection::getTcpSocket() { return tcpSocket; }

std::string Connection::getNick() { return nick; }

void Connection::sendMessage(std::string msg) {
  while (msg.size() < MSG_SIZE) {
    msg += ' ';
  }
  int sentBytes = 0;
  while (sentBytes < MSG_SIZE) {
    int cnt = tcpSocket->write(msg.substr(sentBytes).c_str());
    sentBytes += cnt;
  }
  qInfo() << "wysylam: " << msg.c_str();
}

void Connection::on_connectButton_clicked() {
  tcpSocket->connectToHost(ui->ip->text(), ui->port->text().toInt());
}

void Connection::on_quitButton_clicked() {
  tcpSocket->disconnectFromHost();
  this->close();
}

void Connection::enableConnectButton() {
  if (!ui->table->text().isEmpty() && !ui->nick->text().isEmpty()) {
    ui->connectButton->setEnabled(true);
  } else {
    ui->connectButton->setEnabled(false);
  }
}

void Connection::displayError(QAbstractSocket::SocketError socketError) {
  switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
      QMessageBox::information(this, tr("Draughts"),
                               tr("The server is closed. "
                                  "Please check if server is running"));
      break;
    case QAbstractSocket::HostNotFoundError:
      QMessageBox::information(this, tr("Draughts"),
                               tr("The host was not found. Please check the "
                                  "host name and port settings."));
      break;
    case QAbstractSocket::ConnectionRefusedError:
      QMessageBox::information(this, tr("Draughts"),
                               tr("The connection was refused by the peer. "
                                  "Make sure the server is running, "
                                  "and check that the host name and port "
                                  "settings are correct."));
      break;
    default:
      QMessageBox::information(this, tr("Draughts"),
                               tr("The following error occurred: %1.")
                                   .arg(tcpSocket->errorString()));
  }
}

void Connection::readData() {
  while (tcpSocket->canReadLine()) {
    char buf[100];
    tcpSocket->readLine(buf, 100);
    std::string msg(buf);
    qInfo() << "otrzymuje: " << msg.c_str();
    std::stringstream ss(msg);
    std::string type;
    ss >> type;
    if (type == "ruch") {
      int from, to;
      ss >> from >> to;
      w->getGame()->makeMove(from, to);
    }
    if (type == "sit1") {
      std::string nick;
      ss >> nick;
      w->sitOnPlace1(nick);
    }
    if (type == "sit2") {
      std::string nick;
      ss >> nick;
      w->sitOnPlace2(nick);
    }
    if (type == "sit1ok") {
      w->sitOnPlace1(nick);
      playerPlace = 1;
    }
    if (type == "sit2ok") {
      w->sitOnPlace2(nick);
      playerPlace = 2;
    }
    if (type == "leave1") {
      if (playerPlace == 1) playerPlace = 0;
      w->sitOnPlace1("Empty");
    }
    if (type == "leave2") {
      if (playerPlace == 2) playerPlace = 0;
      w->sitOnPlace2("Empty");
    }
    if (type == "enableStart") {
      w->enableStart(true);
    }
    if (type == "disableStart") {
      w->enableStart(false);
    }
    if (type == "start") {
      w->getGame()->startGame(playerPlace == 2 ? Color::BLACK : Color::WHITE);
      if (playerPlace != 0) {
        w->enableStart(false);
        w->enableDraw(true);
        w->enableResign(true);
      }
    }
    if (type == "drawOffer") {
      w->showDrawOffer(true);
    }
    if (type == "end") {
      int result;
      ss >> result;
      QMessageBox msgBox;
      if (playerPlace != 0) {
        w->enableStart(true);
        w->enableDraw(false);
        w->enableResign(false);
        w->showDrawOffer(false);
      }
      if (result == 1) {
        msgBox.setText("remis");
      } else {
        result == 2 ? msgBox.setText("wygral #1") : msgBox.setText("wygral #2");
        ;
      }
      msgBox.exec();
      w->getGame()->endGame();
    }
  }
}

void Connection::connected() {
  int enableKeepAlive = 1;
  int fd = tcpSocket->socketDescriptor();
  setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &enableKeepAlive,
             sizeof(enableKeepAlive));
  int maxIdle = 10; /* seconds */
  setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &maxIdle, sizeof(maxIdle));
  int count =
      3;  // send up to 3 keepalive packets out, then disconnect if no response
  setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &count, sizeof(count));
  int interval = 2;  // send a keepalive packet out every 2 seconds (after the 5
                     // second idle period)
  setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));

  qInfo() << "polaczono";
  nick = ui->nick->text().toStdString();
  w->enableDraw(false);
  w->enableStart(false);
  w->enableResign(false);
  w->showDrawOffer(false);
  w->getGame()->newGame(Color::WHITE);
  playerPlace = 0;
  this->hide();
  w->show();
  sendMessage(ui->table->text().toStdString());
  sendMessage(nick);
}

void Connection::disconnected() {
  w->hide();
  this->show();
  qInfo() << "rozlaczono polaczenie";
}

int Connection::getplayerPlace() { return playerPlace; }
