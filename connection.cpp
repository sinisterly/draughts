#include "connection.h"
#include "ui_connection.h"
#include <QMessageBox>
#include <sstream>

Connection::Connection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connection),
    tcpSocket(new QTcpSocket(this)),
    w(new MainWindow)
{
    ui->setupUi(this);
    w->getGame()->setConnection(this);
    connect(tcpSocket, &QIODevice::readyRead, this, &Connection::readData);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this, &Connection::displayError);
}

Connection::~Connection()
{
    delete ui;
}

void Connection::sendMove(std::string msg)
{
    tcpSocket->write(msg.c_str());
}

QTcpSocket *Connection::getTcpSocket()
{
    return tcpSocket;
}

std::string Connection::getNick()
{
    return nick;
}

void Connection::setplayerPlace(int playerPlace)
{
    this->playerPlace=playerPlace;
}

void Connection::on_connectButton_clicked()
{
    tcpSocket->connectToHost(ui->ip->text(),ui->port->text().toInt());
    if(tcpSocket->waitForConnected(1000))
    {
        nick=ui->nick->text().toStdString();
        this->hide();
        w->show();
        tcpSocket->write((ui->table->text().toStdString() + '\n').c_str());
        tcpSocket->write((nick + '\n').c_str());
    }
}

void Connection::on_quitButton_clicked()
{
    tcpSocket->disconnectFromHost();
    this->close();
}

void Connection::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
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

void Connection::readData()
{
    char buf[100];
    while(tcpSocket->readLine(buf,100)>0)
    {
        std::string msg(buf);
        qInfo() << msg.c_str();
        std::stringstream ss(msg);
        std::string type;
        ss >> type;
        if(type=="ruch")
        {
            int from,to;
            ss >> from >> to;
            w->getGame()->makeMove(from,to);
        }
        if(type=="sit1")
        {
            std::string nick;
            ss >> nick;
            w->sitOnPlace1(nick);
        }
        if(type=="sit2")
        {
            std::string nick;
            ss >> nick;
            w->sitOnPlace2(nick);
        }
        if(type=="sit1ok")
        {
            w->sitOnPlace1(nick);
            playerPlace=1;
        }
        if(type=="sit2ok")
        {
            w->sitOnPlace2(nick);
            playerPlace=2;
        }
        if(type=="leave1")
        {
            w->sitOnPlace1("Empty");
        }
        if(type=="leave2")
        {
            w->sitOnPlace2("Empty");
        }
        if(type=="enableStart")
        {
            w->enableStart(true);
        }
        if(type=="disableStart")
        {
            w->enableStart(false);
        }
        if(type=="start")
        {
            w->getGame()->startGame(playerPlace==2 ? Color::BLACK : Color::WHITE);
            if(playerPlace!=0)
            {
                w->enableStart(false);
                w->enableDraw(true);
                w->enableResign(true);
            }
        }
        if(type=="drawOffer")
        {
            w->showDrawOffer();

        }
        if(type=="end")
        {
            int result;
            ss >> result;
            qInfo() << "result: " << result;
            QMessageBox msgBox;
            if(playerPlace!=0)
            {
                w->enableStart(true);
                w->enableDraw(false);
                w->enableResign(false);
            }
            if(result==1)
            {
                msgBox.setText("remis");
            }
            else
            {
                result==2 ? msgBox.setText("wygral #1") : msgBox.setText("wygral #2");;
            }
            msgBox.exec();
            w->getGame()->endGame();
        }
    }
}

int Connection::getplayerPlace()
{
    return playerPlace;
}


