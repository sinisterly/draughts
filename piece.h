#ifndef PIECE_H
#define PIECE_H

#include "enum.h"
#include <QGraphicsItem>
#include <QGraphicsScene>


class Piece
{
private:
    PieceType type;
    Color color;
    QGraphicsEllipseItem *ellipse;
public:
    Piece();
    Piece(PieceType type,Color color);
    void addPiece(QGraphicsRectItem *rectangle,PieceType type,Color color);
};

#endif // PIECE_H
