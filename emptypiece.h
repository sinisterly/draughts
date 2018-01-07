#ifndef EMPTYPIECE_H
#define EMPTYPIECE_H

#include "piece.h"

class EmptyPiece : public Piece
{
public:
    explicit EmptyPiece(Color color);
    void addEdges(int x,int y) override;
    void eraseEdges(int,int) override{return;}
    void addMoves(std::vector<std::pair<int,int>>&,int,int) override{return;}
    int moveLength(int,int) override{return 0;}
    void paint(QPainter*) override{return;}
};

#endif // EMPTYPIECE_H
