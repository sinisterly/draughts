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
    QPointF position;
    Piece *piece;
public:
    Square(int x,int y);
    int getIndex();
    Piece* getPiece();
    QPointF getPosition();
    void addPiece(PieceType type,Color color);
    void updatePiece();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
};

#endif // SQUARE_H
