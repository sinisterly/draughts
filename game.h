#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "square.h"
#include "enum.h"
#include <QGraphicsScene>

class Square;
class Game :public QGraphicsScene
{
private:
    Square *board[10][10];
    Player player1;
    Player player2;
    Color turn;
    class Proxy{
    public:
        Proxy(Square** array);
        Square* operator[](int index);
    private:
        Square** array;
    };
public:
    Game();
    Color getTurn();
    void newGame();
    void changeTurn();
    int moveLength();
    void removeCapturedPieces();
    std::vector<std::vector<std::tuple<int,int,int>>> possibleMoves(int length);
    std::vector<std::pair<int,int>> normalMoves();
    std::vector<int> capturedPieces;
    std::vector <std::tuple<int,int,int>> moves[20];
    Proxy operator[](int index);
};

#endif // GAME_H
