#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "square.h"

class Game
{
private:
    Square board[10][10];
    Player player1;
    Player player2;
    QGraphicsScene *scene;
public:
    Game(QGraphicsScene *scene);
    void newGame();
    void loop();
};

#endif // GAME_H
