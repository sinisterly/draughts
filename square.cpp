#include "square.h"
#include <QDebug>

Square::Square(int x,int y)
    :x(x),y(y)
{
    setPos(50*y,50*x);
    piece=new Piece();
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
    if((x+y)%2==0) return -1;
    return 5*x+y/2+1;
}

Color Square::getPieceColor()
{
    return piece->getColor();
}

Piece *Square::getPiece()
{
    return piece;
}

void Square::updatePiece()
{
    auto x=childItems();
    if(x.empty()){
        piece=nullptr;
    }
    else
        piece=qgraphicsitem_cast<Piece*>(childItems()[0]);
}

Square::Square()
{

}

