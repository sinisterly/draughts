#ifndef EMPTYPIECE_H
#define EMPTYPIECE_H

#include "piece.h"

class EmptyPiece : public Piece
{
public:
    EmptyPiece(Color color);
    void addEdges(int x,int y) override;
    void eraseEdges(int x,int y) override{return;}
    void addMoves(std::vector<std::pair<int,int>> &v,int x,int y) override{return;}
    int moveLength(int pos,int depth=0) override{return 0;}
    void paint(QPainter *painter) override{return;}
};

#endif // EMPTYPIECE_H
