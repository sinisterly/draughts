#include "square.h"

Square::Square(QGraphicsScene *scene,int x,int y)
    :x(x),y(y)
{
    QBrush whiteBrush(Qt::white);
    QBrush blackBrush(Qt::black);
    QBrush redBrush(Qt::red);
    QPen outlinePen(Qt::black);
    if((x+y)%2==1)
        rectangle=scene->addRect(0,0,50,50,outlinePen,blackBrush);
    else
        rectangle=scene->addRect(0,0,50,50,outlinePen,whiteBrush);
    rectangle->setPos(50*y,50*x);
}

void Square::addPiece(PieceType type, Color color)
{
    piece.addPiece(rectangle,type,color);
}

Square::Square()
{

}
