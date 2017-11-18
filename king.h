#ifndef KING_H
#define KING_H

#include "piece.h"

class King : public Piece
{
private:
    void addEdge(int x,int y,int a,int b);
    void eraseEdge(int x,int y,int a,int b);
    void addMove(std::vector<std::pair<int,int>> &v,int x,int y,int a,int b);
public:
    King(Color color);
    void addEdges(int x,int y) override;
    void eraseEdges(int x,int y) override;
    void addMoves(std::vector<std::pair<int,int>> &v,int x,int y) override;
    int moveLength(int pos,int depth=0) override;
    void paint(QPainter *painter) override;
    static std::vector<std::pair<int,int>> g[51];
};

#endif // KING_H
