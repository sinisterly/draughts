#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene=new Game();
    ui->graphicsView->setScene(scene);
    /*
    rect1=new Piece();
    scene->addItem(rect1);
    rect2=new Piece();
    rect2->setPos(200,200);
    scene->addItem(rect2);
    */
/*
    QBrush whiteBrush(Qt::white);
    QBrush blackBrush(Qt::black);
    QBrush redBrush(Qt::red);
    QPen outlinePen(Qt::black);
    */
}

MainWindow::~MainWindow()
{
    delete ui;
}
