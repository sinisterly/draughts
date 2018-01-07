#include "man.h"
#include "enum.h"

std::vector<std::pair<int,int>> Man::g[51];

Man::Man(Color color)
    :Piece(color)
{

}

void Man::addEdge(int x,int y,int a,int b)
{
    int k=2;
    Color opposite=game->getTurn()==Color::WHITE ? Color::BLACK : Color::WHITE;
    if(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            game[0][x+a][y+b]->getPiece()->getColor()==opposite &&
            game[0][x+k*a][y+k*b]->getPiece()->getColor()==Color::NONE)
    {
        g[game[0][x][y]->getIndex()].push_back({game[0][x+k*a][y+k*b]->getIndex(),game[0][x+a][y+b]->getIndex()});
        g[game[0][x+k*a][y+k*b]->getIndex()].push_back({game[0][x][y]->getIndex(),game[0][x+a][y+b]->getIndex()});
    }
}

void Man::addEdges(int x,int y)
{
    addEdge(x,y,1,1);
    addEdge(x,y,1,-1);
    addEdge(x,y,-1,1);
    addEdge(x,y,-1,-1);
}

void Man::eraseEdge(int x,int y,int a,int b)
{
    int k=2;
    Color opposite=game->getTurn()==Color::WHITE ? Color::BLACK : Color::WHITE;
    if(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            game[0][x+a][y+b]->getPiece()->getColor()==opposite &&
            game[0][x+k*a][y+k*b]->getPiece()->getColor()==Color::NONE)
    {
        g[game[0][x][y]->getIndex()].pop_back();
        g[game[0][x+k*a][y+k*b]->getIndex()].pop_back();
    }
}

void Man::eraseEdges(int x,int y)
{
    eraseEdge(x,y,1,1);
    eraseEdge(x,y,1,-1);
    eraseEdge(x,y,-1,1);
    eraseEdge(x,y,-1,-1);
}

int Man::moveLength(int pos,int depth)
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

void Man::addMove(std::vector<std::pair<int,int>> &v,int x,int y,int a,int b)
{
    if(x+a<10 && x+a>=0 && y+b<10 && y+b>=0 &&
            game[0][x+a][y+b]->getPiece()->getColor()==Color::NONE)
    {
        v.push_back({game[0][x][y]->getIndex(),game[0][x+a][y+b]->getIndex()});
    }
}

void Man::addMoves(std::vector<std::pair<int, int> > &v, int x, int y)
{
    if(game->getTurn()==Color::WHITE)
    {
        addMove(v,x,y,-1,-1);
        addMove(v,x,y,-1,1);
    }
    else
    {
        addMove(v,x,y,1,1);
        addMove(v,x,y,1,-1);
    }
}

void Man::paint(QPainter *painter)
{
    QRectF rect=boundingRect();
    painter->drawEllipse(rect);
}
