#include <QApplication>
#include "connection.h"
#include "game.h"
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  Connection conn;
  conn.show();
  return a.exec();
}
