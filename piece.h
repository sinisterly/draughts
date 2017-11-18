#ifndef PIECE_H
#define PIECE_H

#include "enum.h"
#include "game.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <vector>

class Game;
class Piece :public QGraphicsItem
{
private:
    Color color;
protected:
    static int vis[51];
public:
    static Game *game;
    Piece(Color color);
    Color getColor();
    virtual void addEdges(int x,int y)=0;
    virtual void eraseEdges(int x,int y)=0;
    virtual int moveLength(int pos,int depth=0)=0;
    virtual void paint(QPainter *painter)=0;
    virtual void addMoves(std::vector<std::pair<int,int>> &v,int x,int y)=0;
    QRectF boundingRect() const override;
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // PIECE_H
