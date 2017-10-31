#include "piece.h"
#include "square.h"
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

Piece::Piece(PieceType type,Color color)
    :type(type),color(color)
{
    pressed=false;
    setFlag(QGraphicsItem::ItemIsMovable);
    setPos(5,5);
}

void Piece::setPiece(PieceType type, Color color)
{
    //this->type=type;
    //this->color=color;
}

QRectF Piece::boundingRect() const
{
    return QRectF(0,0,40,40);
}

void Piece::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rect=boundingRect();
    QPen pen(Qt::black, 3);

    QBrush brush(Qt::red);
    painter->setPen(pen);
    if(color==Color::WHITE)
        painter->setBrush(Qt::white);
    if(color==Color::BLACK)
        painter->setBrush(Qt::red);
    painter->drawEllipse(rect);

}

Color Piece::getColor()
{
    return color;
}

void Piece::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed=true;
    parentItem()->setZValue(1);
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Piece::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Square *parent=qgraphicsitem_cast<Square*>(parentItem());
    parent->setZValue(0);
    pressed=false;
    int from=parent->getIndex();
    Square *newParent;
    foreach(QGraphicsItem *item,scene()->items(mapToScene(event->pos()))){
        Square *node=qgraphicsitem_cast<Square*>(item);
        if(!node || node==qgraphicsitem_cast<Square*>(this))
            continue;
        newParent=node;
        //setParentItem(node);
        //setPos(5,5);
    }
    int to=newParent->getIndex();
    qDebug() << from << ' ' << to;
    setParentItem(newParent);
    setPos(5,5);
    //if(item->parentItem())
        //item=item->parentItem();
    //qDebug() << newParent << (mapToScene(event->pos()));
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

Piece::Piece()
{
    pressed=false;
    setFlag(QGraphicsItem::ItemIsMovable);
    setPos(5,5);
    type=PieceType::NONE;
    color=Color::NONE;
}
