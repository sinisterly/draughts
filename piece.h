#ifndef PIECE_H
#define PIECE_H

#include "enum.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>


class Piece :public QGraphicsItem
{
private:
    PieceType type;
    Color color;
public:
    bool pressed;
    Piece();
    Piece(PieceType type,Color color);
    void setPiece(PieceType type,Color color);
    QRectF boundingRect() const;
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget);
    Color getColor();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // PIECE_H
