#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include "piece.h"
#include "game.h"

namespace Ui {
class MainWindow;
}

class Game;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Game* getGame();
    void sitOnPlace1(const std::string &nick);
    void sitOnPlace2(const std::string &nick);
    void enableStart(bool enable);
    void enableDraw(bool enable);
    void enableResign(bool enable);
    void showDrawOffer();


private slots:
    void on_place1Button_clicked();

    void on_place2Button_clicked();

    void on_startButton_clicked();

    void on_drawButton_clicked();

    void on_resignButton_clicked();

    void on_drawYes_clicked();

    void on_drawNo_clicked();

private:
    Ui::MainWindow *ui;
    Game *game;
    void closeEvent (QCloseEvent *event);


};

#endif // MAINWINDOW_H
