#include "piece.h"

Piece::Piece(PieceType type,Color color)
    :type(type),color(color)
{

}

void Piece::addPiece(QGraphicsRectItem *rectangle, PieceType type, Color color)
{
    this->type=type;
    this->color=color;
    if(color==Color::NONE) return;
    QBrush whiteBrush(Qt::white);
    QBrush blackBrush(Qt::black);
    QBrush redBrush(Qt::red);
    QPen outlinePen(Qt::black);
    ellipse=new QGraphicsEllipseItem(rectangle);
    ellipse->setPos(5,5);
    ellipse->setRect(0,0,40,40);
    ellipse->setPen(outlinePen);
    if(color==Color::BLACK)
        ellipse->setBrush(redBrush);
    else
        ellipse->setBrush(whiteBrush);
    ellipse->setFlags(QGraphicsItem::ItemIsMovable);
}

Piece::Piece()
{
    type=PieceType::NONE;
    color=Color::NONE;
}
