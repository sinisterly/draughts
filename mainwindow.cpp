#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsSceneMouseEvent>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->drawButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->resignButton->setEnabled(false);
    ui->drawOffer->hide();
    game=new Game();
    ui->graphicsView->setScene(game);
}

MainWindow::~MainWindow()
{
    delete ui;
}

Game *MainWindow::getGame()
{
    return game;
}

void MainWindow::sitOnPlace1(const std::string &nick)
{
    ui->place1Button->setText(QString::fromStdString(nick));
}

void MainWindow::sitOnPlace2(const std::string &nick)
{
    ui->place2Button->setText(QString::fromStdString(nick));
}

void MainWindow::enableStart(bool enable)
{
    ui->startButton->setEnabled(enable);
}

void MainWindow::enableDraw(bool enable)
{
    ui->drawButton->setEnabled(enable);
}

void MainWindow::enableResign(bool enable)
{
    ui->resignButton->setEnabled(enable);
}

void MainWindow::showDrawOffer()
{
    ui->drawOffer->show();
}

void MainWindow::on_place1Button_clicked()
{
    if(game->getConnection()->getplayerPlace()==0)
    {
        game->getConnection()->sendMessage("sit1 " + game->getConnection()->getNick());
    }
    else if(game->getConnection()->getplayerPlace()==1)
    {
        game->getConnection()->sendMessage("leave1");
    }
}

void MainWindow::on_place2Button_clicked()
{
    if(game->getConnection()->getplayerPlace()==0)
    {
        game->getConnection()->sendMessage("sit2 " + game->getConnection()->getNick());
    }
    else if(game->getConnection()->getplayerPlace()==2)
    {
        game->getConnection()->sendMessage("leave2");
    }
}

void MainWindow::on_startButton_clicked()
{
    game->getConnection()->sendMessage("start");
}

void MainWindow::on_drawButton_clicked()
{
    game->getConnection()->sendMessage("drawOffer");

}

void MainWindow::on_resignButton_clicked()
{
    game->getConnection()->sendMessage("resign");
}

void MainWindow::on_drawYes_clicked()
{
    ui->drawOffer->hide();
    game->getConnection()->sendMessage("drawYes");
}

void MainWindow::on_drawNo_clicked()
{
    ui->drawOffer->hide();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    game->getConnection()->getTcpSocket()->disconnectFromHost();
    game->getConnection()->show();
    //MainWindow::closeEvent(event);
}
