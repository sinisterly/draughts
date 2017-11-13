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
        for(int j=i&1^1;j<10;j+=2)
            board[i][j]->setPiece(PieceType::MAN,Color::BLACK);
    for(int i=6;i<10;i++)
        for(int j=i&1^1;j<10;j+=2)
            board[i][j]->setPiece(PieceType::MAN,Color::WHITE);
    turn=Color::WHITE;
    isCapturing=false;
}

std::vector<std::pair<int,int>> g[51];
std::vector<std::pair<int,int>> g2[51];
int vis[51];

void Game::addEdge(int x,int y,int a,int b)
{
    int k=2;
    Color opposite=turn==Color::WHITE ? Color::BLACK : Color::WHITE;
    if(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            board[x+a][y+b]->getPieceColor()==opposite &&
            board[x+k*a][y+k*b]->getPieceColor()==Color::NONE)
    {
        g[board[x][y]->getIndex()].push_back({board[x+k*a][y+k*b]->getIndex(),board[x+a][y+b]->getIndex()});
        g[board[x+k*a][y+k*b]->getIndex()].push_back({board[x][y]->getIndex(),board[x+a][y+b]->getIndex()});
    }
}

void Game::addEdge2(int x,int y,int a,int b)
{
    Color opposite=turn==Color::WHITE ? Color::BLACK : Color::WHITE;
    int l=1;
    while(x+l*a>=0 && x+l*a<10 && y+l*b>=0 && y+l*b<10 &&
          board[x+l*a][y+l*b]->getPieceColor()==Color::NONE) l++;
    if(!(x+l*a>=0 && x+l*a<10 && y+l*b>=0 && y+l*b<10) ||
            board[x+l*a][y+l*b]->getPieceColor()!=opposite) return;
    int k=l+1;
    while(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            board[x+k*a][y+k*b]->getPieceColor()==Color::NONE)
    {
        g2[board[x][y]->getIndex()].push_back({board[x+k*a][y+k*b]->getIndex(),board[x+l*a][y+l*b]->getIndex()});
        g2[board[x+k*a][y+k*b]->getIndex()].push_back({board[x][y]->getIndex(),board[x+l*a][y+l*b]->getIndex()});
        k++;
    }
}

void Game::eraseEdge(int x,int y,int a,int b)
{
    int k=2;
    Color opposite=turn==Color::WHITE ? Color::BLACK : Color::WHITE;
    if(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            board[x+a][y+b]->getPieceColor()==opposite &&
            board[x+k*a][y+k*b]->getPieceColor()==Color::NONE)
    {
        g[board[x][y]->getIndex()].pop_back();
        g[board[x+k*a][y+k*b]->getIndex()].pop_back();
    }
}

void Game::eraseEdge2(int x,int y,int a,int b)
{
    Color opposite=turn==Color::WHITE ? Color::BLACK : Color::WHITE;
    int l=1;
    if(!(x+l*a>=0 && x+l*a<10 && y+l*b>=0 && y+l*b<10) ||
            board[x+l*a][y+l*b]->getPieceColor()!=opposite) return;
    if(board[x+l*a][y+l*b]->getPieceColor()!=opposite) return;
    int k=l+1;
    while(x+k*a>=0 && x+k*a<10 && y+k*b>=0 && y+k*b<10 &&
            board[x+k*a][y+k*b]->getPieceColor()==Color::NONE)
    {
        g2[board[x][y]->getIndex()].pop_back();
        g2[board[x+k*a][y+k*b]->getIndex()].pop_back();
        k++;
    }
}

void Game::removeCapturedPieces()
{
    for(auto i:capturedPieces)
    {
        int x=(i-1)/5;
        int y=((i-1)%5)*2+(x&1^1);
        board[x][y]->setPiece(PieceType::NONE,Color::NONE);
        board[x][y]->update();
    }
    capturedPieces.clear();
}

int Game::moveLength()
{
    for(int i=1;i<=50;i++)
    {
        g[i].clear();
        g2[i].clear();
    }
    for(int i=0;i<10;i++)
        for(int j=i&1^1;j<10;j+=2)
            if(board[i][j]->getPiece()->getColor()==Color::NONE)
            {
                    addEdge(i,j,1,1);
                    addEdge(i,j,1,-1);
                    addEdge(i,j,-1,1);
                    addEdge(i,j,-1,-1);
                    addEdge2(i,j,1,1);
                    addEdge2(i,j,1,-1);
                    addEdge2(i,j,-1,1);
                    addEdge2(i,j,-1,-1);
            }
    int length=0;
    std::fill(tab,tab+51,0);
    for(int i=0;i<10;i++)
        for(int j=i&1^1;j<10;j+=2)
            if(board[i][j]->getPieceColor()==turn){
                if(board[i][j]->getPiece()->getPieceType()==PieceType::MAN)
                {
                    std::fill(vis,vis+51,0);
                    addEdge(i,j,1,1);
                    addEdge(i,j,1,-1);
                    addEdge(i,j,-1,1);
                    addEdge(i,j,-1,-1);
                    int pos=board[i][j]->getIndex();
                    tab[pos]=moveLength(pos);
                    length=std::max(length,moveLength(pos));
                    eraseEdge(i,j,1,1);
                    eraseEdge(i,j,1,-1);
                    eraseEdge(i,j,-1,1);
                    eraseEdge(i,j,-1,-1);
                }
                if(board[i][j]->getPiece()->getPieceType()==PieceType::KING)
                {
                    std::fill(vis,vis+51,0);
                    addEdge2(i,j,1,1);
                    addEdge2(i,j,1,-1);
                    addEdge2(i,j,-1,1);
                    addEdge2(i,j,-1,-1);
                    int pos=board[i][j]->getIndex();
                    tab[pos]=moveLength2(pos);
                    length=std::max(length,moveLength2(pos));
                    eraseEdge2(i,j,1,1);
                    eraseEdge2(i,j,1,-1);
                    eraseEdge2(i,j,-1,1);
                    eraseEdge2(i,j,-1,-1);
                }
            }
    return length;
}

std::vector <std::tuple<int,int,int>> moves[20];

int Game::moveLength(int pos,int depth)
{
    int length=0;
    for(auto to:g[pos]){
        if(vis[to.second])
            continue;
        vis[to.second]=1;
        length=std::max(length,moveLength(to.first,depth+1)+1);
        moves[depth].push_back(std::make_tuple(pos,to.first,to.second));
        vis[to.second]=0;
    }
    return length;
}

int Game::moveLength2(int pos,int depth)
{
    int length=0;
    for(auto to:g2[pos]){
        if(vis[to.second])
            continue;
        vis[to.second]=1;
        length=std::max(length,moveLength2(to.first,depth+1)+1);
        moves[depth].push_back(std::make_tuple(pos,to.first,to.second));
        vis[to.second]=0;
    }
    return length;
}

void Game::addMove(std::vector<std::pair<int,int>> &v,int x,int y,int a,int b)
{
    int k=1;
    int l=board[x][y]->getPiece()->getPieceType()==PieceType::MAN ? 1 : 10;
    while(k<=l && x+k*a<10 && x+k*a>=0 && y+k*b<10 && y+k*b>=0 &&
            board[x+k*a][y+k*b]->getPieceColor()==Color::NONE)
    {
        v.push_back({board[x][y]->getIndex(),board[x+k*a][y+k*b]->getIndex()});
        k++;
    }
}

std::vector<std::pair<int,int>> Game::normalMoves()
{
    std::vector<std::pair<int,int>> v;
    for(int i=0;i<10;i++)
        for(int j=i&1^1;j<10;j+=2)
            if(board[i][j]->getPieceColor()==turn)
            {
                if(board[i][j]->getPiece()->getPieceType()==PieceType::KING)
                {
                    addMove(v,i,j,-1,-1);
                    addMove(v,i,j,-1,1);
                    addMove(v,i,j,1,1);
                    addMove(v,i,j,1,-1);
                }
                else if(turn==Color::WHITE)
                {
                    addMove(v,i,j,-1,-1);
                    addMove(v,i,j,-1,1);
                }
                else
                {
                    addMove(v,i,j,1,1);
                    addMove(v,i,j,1,-1);
                }
            }
    return v;
}

std::vector<std::vector<std::tuple<int,int,int>>> Game::possibleMoves(int length)
{
    std::vector<std::vector<std::tuple<int,int,int>>> v;
    v.resize(length+1);
    v[length-1]=moves[length-1];
    for(int i=length-1;i>0;i--)
        for(auto j:v[i])
            for(auto k:moves[i-1])
                if(std::get<0>(j)==std::get<1>(k))
                    v[i-1].push_back(k);
    return v;
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

void Game::changeTurn()
{
    turn = turn==Color::WHITE ? Color::BLACK : Color::WHITE;
}

