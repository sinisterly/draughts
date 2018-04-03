#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>
#include <QtNetwork>
#include "mainwindow.h"

namespace Ui {
class Connection;
}

class MainWindow;
class Connection : public QDialog {
  Q_OBJECT

 public:
  explicit Connection(QWidget *parent = 0);
  ~Connection();
  void sendMove(const std::string &msg);
  QTcpSocket *getTcpSocket();
  std::string getNick();
  int getplayerPlace();
  void sendMessage(std::string msg);

 private slots:
  void on_connectButton_clicked();
  void on_quitButton_clicked();
  void enableConnectButton();
  void displayError(QAbstractSocket::SocketError socketError);
  void readData();
  void connected();
  void disconnected();

 private:
  Ui::Connection *ui;
  QTcpSocket *tcpSocket;
  MainWindow *w;
  std::string nick;
  int playerPlace = 0;
};

#endif  // CONNECTION_H
