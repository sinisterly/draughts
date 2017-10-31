#include "square.h"
#include <QDebug>

Square::Square(QGraphicsScene *scene,int x,int y)
    :x(x),y(y)
{
    setPos(50*y,50*x);
    scene->addItem(this);
}

void Square::addPiece(PieceType type, Color color)
{
    piece=new Piece(type,color);
    piece->setParentItem(this);
}

void Square::removePiece()
{
    //piece=nullptr;
}

void Square::setPiece(PieceType type, Color color)
{
    piece->setPiece(type,color);
}

QRectF Square::boundingRect() const
{
    return QRectF(0,0,50,50);
}

void Square::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rect=boundingRect();
    QPen pen(Qt::black, 3);
    painter->setPen(pen);
    if((x+y)%2)
       painter->setBrush(QBrush(Qt::black));
    else
        painter->setBrush(QBrush(Qt::white));
    painter->drawRect(rect);
}

int Square::getIndex()
{
    return 5*x+y/2+1;
}

Piece *Square::getPiece()
{
    return piece;
}

Square::Square()
{

}

