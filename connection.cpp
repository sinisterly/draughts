#include "connection.h"
#include "ui_connection.h"
#include <QMessageBox>
#include <sstream>
#include <cassert>

Connection::Connection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Connection),
    tcpSocket(new QTcpSocket(this)),
    w(new MainWindow)
{
    ui->setupUi(this);
    ui->table->setValidator(new QIntValidator(0, 199, this) );
    ui->port->setValidator(new QIntValidator(0, 65535, this) );
    w->getGame()->setConnection(this);
    connect(tcpSocket, &QIODevice::readyRead, this, &Connection::readData);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this, &Connection::displayError);
    connect(tcpSocket, &QAbstractSocket::disconnected, this, &Connection::disconnect);
}

Connection::~Connection()
{
    delete ui;
}

void Connection::sendMove(std::string msg)
{
    sendMessage(msg);
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

void Connection::sendMessage(std::string msg)
{
    while(msg.size()<MSG_SIZE)
    {
        msg+=' ';
    }
    assert(msg.size()==MSG_SIZE);
    int sentBytes=0;
    while(sentBytes<MSG_SIZE)
    {
        int cnt=tcpSocket->write(msg.substr(sentBytes).c_str());
        sentBytes+=cnt;
    }
    assert(sentBytes==MSG_SIZE);
}

void Connection::on_connectButton_clicked()
{
    tcpSocket->connectToHost(ui->ip->text(),ui->port->text().toInt());
    if(tcpSocket->waitForConnected(1000))
    {
        nick=ui->nick->text().toStdString();
        this->hide();
        delete w;
        w=new MainWindow();
        w->getGame()->setConnection(this);
        playerPlace=0;
        w->show();
        sendMessage(ui->table->text().toStdString());
        sendMessage(nick);
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
        QMessageBox::information(this, tr("Draughts"),
                                 tr("The server is closed"
                                    "Please check if server is running"));
            w->hide();
            this->show();
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
    while(tcpSocket->canReadLine())
    {
        char buf[100];
        tcpSocket->readLine(buf,100);
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

void Connection::disconnect()
{
    qInfo() << "rozlaczono polaczenie";
}

int Connection::getplayerPlace()
{
    return playerPlace;
}


