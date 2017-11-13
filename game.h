#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "square.h"
#include "enum.h"

class Game :public QGraphicsScene
{
private:
    Square *board[10][10];
    int tab[51];
    Player player1;
    Player player2;
    Color turn;
public:
    Game();
    void newGame();
    void loop();
    void changeTurn();
    int moveLength();
    int moveLength(int pos,int depth=0);
    int moveLength2(int pos,int depth=0);
    std::vector<std::vector<std::tuple<int,int,int>>> possibleMoves(int length);
    std::vector<std::pair<int,int>> normalMoves();
    void addMove(std::vector<std::pair<int,int>> &v,int x,int y,int a,int b);
    void addEdge(int x,int y,int a,int b);
    void addEdge2(int x,int y,int a,int b);
    void eraseEdge(int x,int y,int a,int b);
    void eraseEdge2(int x,int y,int a,int b);
    void getWhite();
    bool isCapturing;
    int totalCaptureLength;
    int currentCaptureLength;
    std::vector<int> capturedPieces;
    void removeCapturedPieces();
    friend void Piece::mousePressEvent(QGraphicsSceneMouseEvent *event);
    friend void Piece::mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // GAME_H
