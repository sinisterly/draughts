#include "king.h"

std::vector<std::pair<int,int>> King::g[51];



King::King(Color color)
    :Piece(color)
{

}

void King::addEdge(int x,int y,int a,int b)
{
    Color opposite=game->getTurn()==Color::WHITE ? Color::BLACK : Color::WHITE;
    int l=1;
    while(x+l*a>=0 && x+l*a<10 && y+l*b>=0 && y+l*b<10 &&
          game[0][x+l*a][y+l*b]->getPiece()->getColor()==Color::NONE) l++;
    if(!(x+l*a>=0 && x+l*a<10 && y+l*b>=0 && y+l*b<10 &&
            game[0][x+l*a][y+l*b]->getPiece()->getColor()==opposite)) return;
    int k=l+1;
    while(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            game[0][x+k*a][y+k*b]->getPiece()->getColor()==Color::NONE)
    {
        g[game[0][x][y]->getIndex()].push_back({game[0][x+k*a][y+k*b]->getIndex(),game[0][x+l*a][y+l*b]->getIndex()});
        g[game[0][x+k*a][y+k*b]->getIndex()].push_back({game[0][x][y]->getIndex(),game[0][x+l*a][y+l*b]->getIndex()});
        k++;
    }
}

void King::addEdges(int x,int y)
{
    addEdge(x,y,1,1);
    addEdge(x,y,1,-1);
    addEdge(x,y,-1,1);
    addEdge(x,y,-1,-1);
}

void King::eraseEdge(int x,int y,int a,int b)
{
    Color opposite=game->getTurn()==Color::WHITE ? Color::BLACK : Color::WHITE;
    int l=1;
    if(!(x+l*a>=0 && x+l*a<10 && y+l*b>=0 && y+l*b<10 &&
            game[0][x+l*a][y+l*b]->getPiece()->getColor()==opposite)) return;
    if(game[0][x+l*a][y+l*b]->getPiece()->getColor()!=opposite) return;
    int k=l+1;
    while(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            game[0][x+k*a][y+k*b]->getPiece()->getColor()==Color::NONE)
    {
        g[game[0][x][y]->getIndex()].pop_back();
        g[game[0][x+k*a][y+k*b]->getIndex()].pop_back();
        k++;
    }
}

void King::eraseEdges(int x,int y)
{
    eraseEdge(x,y,1,1);
    eraseEdge(x,y,1,-1);
    eraseEdge(x,y,-1,1);
    eraseEdge(x,y,-1,-1);
}

int King::moveLength(int pos,int depth)
{
    if(depth==0)
        std::fill(vis,vis+51,0);
    int length=0;
    for(auto to:g[pos]){
        if(vis[to.second])
            continue;
        vis[to.second]=1;
        length=std::max(length,moveLength(to.first,depth+1)+1);
        game->moves[depth].push_back(std::make_tuple(pos,to.first,to.second));
        vis[to.second]=0;
    }
    return length;
}

void King::addMove(std::vector<std::pair<int,int>> &v,int x,int y,int a,int b)
{
    int k=1;
    int l=10;
    while(k<=l && x+k*a<10 && x+k*a>=0 && y+k*b<10 && y+k*b>=0 &&
            game[0][x+k*a][y+k*b]->getPiece()->getColor()==Color::NONE)
    {
        v.push_back({game[0][x][y]->getIndex(),game[0][x+k*a][y+k*b]->getIndex()});
        k++;
    }
}

void King::addMoves(std::vector<std::pair<int, int> > &v, int x, int y)
{
    addMove(v,x,y,-1,-1);
    addMove(v,x,y,-1,1);
    addMove(v,x,y,1,1);
    addMove(v,x,y,1,-1);
}

void King::paint(QPainter *painter)
{
    QRectF rect=boundingRect();
    painter->drawRect(rect);
}
