#include "mainwindow.h"
#include "connection.h"
#include <QApplication>
#include "game.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Connection conn;
    conn.show();
    return a.exec();
}
