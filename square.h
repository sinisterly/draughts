#ifndef SQUARE_H
#define SQUARE_H

#include "enum.h"
#include "piece.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QDebug>

class Square :public QGraphicsItem
{
private:
    int x;
    int y;
    Piece *piece;
public:
    Square();
    Square(QGraphicsScene *scene,int x,int y);
    void addPiece(PieceType type,Color color);
    void removePiece();
    void setPiece(PieceType type,Color color);
    QRectF boundingRect() const;
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget);
    int getIndex();
    Piece *getPiece();
};

#endif // SQUARE_H
