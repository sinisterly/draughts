#ifndef SQUARE_H
#define SQUARE_H

#include "enum.h"
#include "piece.h"
#include <QGraphicsItem>
#include <QGraphicsScene>

class Square
{
private:
    int x;
    int y;
    QGraphicsRectItem *rectangle;
    Piece piece;
public:
    Square();
    Square(QGraphicsScene *scene,int x,int y);
    void addPiece(PieceType type,Color color);
};

#endif // SQUARE_H
