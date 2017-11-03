#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "square.h"
#include "enum.h"

class Game
{
private:
    Square *board[10][10];
    Player player1;
    Player player2;
    Color turn;
    QGraphicsScene *scene;
public:
    Game(QGraphicsScene *scene);
    void newGame();
    void loop();
    int moveLength();
    int moveLength(int pos);
    void addEdge(int x,int y,int a,int b,int k);
    void eraseEdge(int x,int y,int a,int b,int k);
};

#endif // GAME_H
