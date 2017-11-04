#include "game.h"
#include "enum.h"

Game::Game()
{
    newGame();
    //loop();
}


void Game::newGame()
{
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            this->addItem(board[i][j]=new Square(i,j));
    for(int i=0;i<=3;i++)
        for(int j=0;j<10;j++)
            if((i+j)%2)
                board[i][j]->setPiece(PieceType::MAN,Color::BLACK);
    for(int i=6;i<10;i++)
        for(int j=0;j<10;j++)
            if((i+j)%2)
                board[i][j]->setPiece(PieceType::MAN,Color::WHITE);
    turn=Color::WHITE;
}

std::vector<std::pair<int,int>> g[51];
int vis[51];

void Game::addEdge(int x,int y,int a,int b,int k)
{
    Color opposite=turn==Color::WHITE ? Color::BLACK : Color::WHITE;
    if(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            board[x+a][y+b]->getPieceColor()==opposite &&
            board[x+k*a][y+k*b]->getPieceColor()==Color::NONE)
    {
        g[board[x][y]->getIndex()].push_back({board[x+k*a][y+k*b]->getIndex(),board[x+a][y+b]->getIndex()});
        g[board[x+k*a][y+k*b]->getIndex()].push_back({board[x][y]->getIndex(),board[x+a][y+b]->getIndex()});
    }
}

void Game::eraseEdge(int x,int y,int a,int b,int k)
{
    Color opposite=turn==Color::WHITE ? Color::BLACK : Color::WHITE;
    if(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            board[x+a][y+b]->getPieceColor()==opposite &&
            board[x+k*a][y+k*b]->getPieceColor()==Color::NONE)
    {
        g[board[x][y]->getIndex()].pop_back();
        g[board[x+k*a][y+k*b]->getIndex()].pop_back();
    }
}

void Game::getWhite()
{
    /*for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            if((i+j)%2)
                if(board[i][j]->getPieceColor()==Color::WHITE)
                    qDebug()<<board[i][j]->getIndex() << ' ';
    */
    qDebug() << moveLength();
}

int Game::moveLength()
{
    for(int i=1;i<=50;i++)
        g[i].clear();
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            if(board[i][j]->getPieceColor()==Color::NONE){
                addEdge(i,j,1,1,2);
                addEdge(i,j,1,-1,2);
                addEdge(i,j,-1,1,2);
                addEdge(i,j,-1,-1,2);
            }
    int length=0;
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            if(board[i][j]->getPieceColor()==turn){
                std::fill(vis,vis+51,0);
                addEdge(i,j,1,1,2);
                addEdge(i,j,1,-1,2);
                addEdge(i,j,-1,1,2);
                addEdge(i,j,-1,-1,2);
                length=std::max(length,moveLength(board[i][j]->getIndex()));
                eraseEdge(i,j,1,1,2);
                eraseEdge(i,j,1,-1,2);
                eraseEdge(i,j,-1,1,2);
                eraseEdge(i,j,-1,-1,2);
            }
    return length;
}

int Game::moveLength(int pos)
{
    int length=0;
    for(auto to:g[pos]){
        if(vis[to.second])
            continue;
        vis[to.second]=1;
        length=std::max(length,moveLength(to.first)+1);
        vis[to.second]=0;
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

