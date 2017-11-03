#include "game.h"
#include "enum.h"


Game::Game(QGraphicsScene *scene)
    :scene(scene){
    newGame();
    //loop();
}


void Game::newGame()
{
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            board[i][j]=new Square(scene,i,j);
    for(int i=0;i<=3;i++)
        for(int j=0;j<10;j++)
            if((i+j)%2==1)
                board[i][j]->addPiece(PieceType::MAN,Color::BLACK);
    for(int i=6;i<10;i++)
        for(int j=0;j<10;j++)
            if((i+j)%2==1)
                board[i][j]->addPiece(PieceType::MAN,Color::WHITE);
    turn=Color::WHITE;
}

std::vector<int> g[51];
int vis[51];

void Game::addEdge(int x,int y,int a,int b,int k)
{
    Color opposite=turn==Color::WHITE ? Color::BLACK : Color::WHITE;
    if(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            board[x+a][y+b]->getPiece()->getColor()==opposite &&
            board[x+k*a][y+k*b]->getPiece()->getColor()==Color::NONE)
    {
        g[board[x][y]->getIndex()].push_back(board[x+k*a][y+k*b]->getIndex());
        g[board[x+k*a][y+k*b]->getIndex()].push_back(board[x][y]->getIndex());
    }
}

void Game::eraseEdge(int x,int y,int a,int b,int k)
{
    Color opposite=turn==Color::WHITE ? Color::BLACK : Color::WHITE;
    if(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            board[x+a][y+b]->getPiece()->getColor()==opposite &&
            board[x+k*a][y+k*b]->getPiece()->getColor()==Color::NONE)
    {
        g[board[x][y]->getIndex()].pop_back();
        g[board[x+k*a][y+k*b]->getIndex()].pop_back();
    }
}

int Game::moveLength()
{
    for(int i=1;i<=50;i++)
        g[i].clear();
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            if(board[i][j]->getPiece()->getColor()==Color::NONE){
                addEdge(i,j,1,1,1);
                addEdge(i,j,1,-1,1);
                addEdge(i,j,-1,1,1);
                addEdge(i,j,-1,-1,1);
            }
    int length=0;
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            if(board[i][j]->getPiece()->getColor()==turn){
                std::fill(vis,vis+51,0);
                addEdge(i,j,1,1,1);
                addEdge(i,j,1,-1,1);
                addEdge(i,j,-1,1,1);
                addEdge(i,j,-1,-1,1);
                length=std::max(length,moveLength(board[i][j]->getIndex()));
                eraseEdge(i,j,1,1,1);
                eraseEdge(i,j,1,-1,1);
                eraseEdge(i,j,-1,1,1);
                eraseEdge(i,j,-1,-1,1);
            }
    return length;
}

int Game::moveLength(int pos)
{
    int length=0;
    vis[pos]=1;
    for(auto to:g[pos]){
        if(vis[to])
            continue;
        length=std::max(length,moveLength(to)+1);
    }
    return length;
}

void Game::loop()
{
    newGame();
    while(1)
    {
        qDebug()<<moveLength();
        turn=turn==Color::WHITE ? Color::BLACK : Color::WHITE;
    }
}

