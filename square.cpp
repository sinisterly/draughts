#include "square.h"
#include "man.h"
#include "king.h"
#include "emptypiece.h"
#include <QDebug>

Square::Square(int x,int y)
{
    index=(x+y)%2 ? 5*x+y/2+1 : -1;
    setPos(50*y,50*x);
    piece=nullptr;
}

void Square::addPiece(PieceType type, Color color)
{
    if(piece)
        delete piece;
    if(type==PieceType::MAN)
        piece=new Man(color);
    else if(type==PieceType::KING)
        piece=new King(color);
    else
        piece=new EmptyPiece(color);
    piece->setParentItem(this);
    piece->setFlag(QGraphicsItem::ItemIsMovable);
    piece->setPos(5,5);
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
    if(index!=-1)
       painter->setBrush(QBrush(Qt::black));
    else
        painter->setBrush(QBrush(Qt::white));
    painter->drawRect(rect);
}

int Square::getIndex()
{
    return index;
}

Piece *Square::getPiece()
{
    return piece;
}

void Square::updatePiece()
{
    auto x=childItems();
    if(x.empty())
        piece=nullptr;
    else
        piece=qgraphicsitem_cast<Piece*>(childItems()[0]);
}

Square::Square()
{

}

