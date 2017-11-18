#ifndef SQUARE_H
#define SQUARE_H

#include "enum.h"
#include "piece.h"
#include <QGraphicsItem>
#include <QGraphicsScene>

class Piece;
class Square :public QGraphicsItem
{
private:
    int index;
    Piece *piece;
public:
    Square();
    Square(int x,int y);
    int getIndex();
    Piece* getPiece();
    void addPiece(PieceType type,Color color);
    void updatePiece();
    QRectF boundingRect() const override;
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

#endif // SQUARE_H
