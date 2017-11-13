#include "piece.h"
#include "square.h"
#include "game.h"
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

void Piece::setPiece(PieceType type, Color color)
{
    this->type=type;
    this->color=color;
}

QRectF Piece::boundingRect() const
{
    return QRectF(0,0,40,40);
}

void Piece::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(color==Color::NONE)
        return;
    QRectF rect=boundingRect();
    QPen pen(Qt::black, 3);

    QBrush brush(Qt::red);
    painter->setPen(pen);
    if(color==Color::WHITE)
        painter->setBrush(Qt::white);
    if(color==Color::BLACK)
        painter->setBrush(Qt::red);
    if(type==PieceType::MAN)
        painter->drawEllipse(rect);
    if(type==PieceType::KING)
        painter->drawRect(rect);
}

Color Piece::getColor()
{
    return color;
}

PieceType Piece::getPieceType()
{
    return type;
}

void Piece::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    pressed=true;
    parentItem()->setZValue(1);
    update();
    QGraphicsItem::mousePressEvent(event);
    Game *game=dynamic_cast<Game*>(scene());
    //game->getWhite();
}

void Piece::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Square *parent=qgraphicsitem_cast<Square*>(parentItem());
    parent->setZValue(0);
    pressed=false;
    int from=parent->getIndex();
    Square *newParent=nullptr;
    foreach(QGraphicsItem *item,scene()->items(mapToScene(event->pos()))){
        Square *node=qgraphicsitem_cast<Square*>(item);
        if(!node || node==qgraphicsitem_cast<Square*>(this))
            continue;
        newParent=node;
        //setParentItem(node);
        //setPos(5,5);
    }
    if(newParent==nullptr)
    {
        setPos(5,5);
        return;
    }
    int to=newParent->getIndex();
    //qDebug() << from << ' ' << to;
    Game *game=dynamic_cast<Game*>(scene());
    if(!(game->isCapturing))
    {
        int length=game->moveLength();
        if(length==0)
        {
            auto moves=game->normalMoves();
            bool okMove=std::find(moves.begin(),moves.end(),std::make_pair(from,to))!=moves.end();
            if(okMove)
            {
                Piece* piece2=newParent->getPiece();
                piece2->setParentItem(parent);
                setParentItem(newParent);
                parent->updatePiece();
                newParent->updatePiece();
                game->changeTurn();
                if((color==Color::WHITE && to<=5) || (color==Color::BLACK && to>=46))
                    type=PieceType::KING;
            }
        }
        else
        {
            game->totalCaptureLength=length;
            game->currentCaptureLength=0;
            game->isCapturing=true;
        }
    }
    if(game->isCapturing)
    {
        auto moves=game->possibleMoves(game->totalCaptureLength);
        auto it=std::find_if(moves[game->currentCaptureLength].begin(),moves[game->currentCaptureLength].end(),
                [&](std::tuple<int,int,int> &x){return std::get<0>(x)==from && std::get<1>(x)==to;});
        bool okMove=(it!=moves[game->currentCaptureLength].end());
        if(okMove)
        {
            Piece* piece2=newParent->getPiece();
            piece2->setParentItem(parent);
            setParentItem(newParent);
            parent->updatePiece();
            newParent->updatePiece();
            game->currentCaptureLength++;
            game->capturedPieces.push_back(std::get<2>(*it));
            if(game->totalCaptureLength==game->currentCaptureLength)
            {
                game->changeTurn();
                game->isCapturing=false;
                game->removeCapturedPieces();
                if((color==Color::WHITE && to<=5) || (color==Color::BLACK && to>=46))
                    type=PieceType::KING;
            }
        }
    }
    //qDebug() << okMove;

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
