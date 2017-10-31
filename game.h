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
};

#endif // GAME_H
