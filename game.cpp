#include "game.h"
#include "man.h"
#include "king.h"
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <sstream>

Color Game::getColor() const
{
    return color;
}

void Game::setColor(const Color &value)
{
    color = value;
}

Game::Game()
{
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            this->addItem(board[i][j]=new Square(i,j));
    newGame(Color::WHITE);
}

Game* Piece::game;

void Game::newGame(Color color)
{
    this->color=color;
    turn=color;
    Piece::game=this;
    for(int i=0;i<=3;i++)
        for(int j=i%2^1;j<10;j+=2)
            board[i][j]->addPiece(PieceType::MAN,Color::BLACK);
    for(int i=4;i<=5;i++)
        for(int j=i%2^1;j<10;j+=2)
            board[i][j]->addPiece(PieceType::NONE,Color::NONE);
    for(int i=6;i<10;i++)
        for(int j=i%2^1;j<10;j+=2)
            board[i][j]->addPiece(PieceType::MAN,Color::WHITE);
}

void Game::startGame(Color color)
{
    newGame(color);
    for(int i=0;i<10;i++)
        for(int j=i%2^1;j<10;j+=2)
            if(conn->getplayerPlace()!=0 && board[i][j]->getPiece()->getColor()==turn)
                board[i][j]->getPiece()->setFlag(QGraphicsItem::ItemIsMovable);
}

void Game::endGame()
{
    for(int i=0;i<10;i++)
        for(int j=i%2^1;j<10;j+=2)
            board[i][j]->getPiece()->setFlag(QGraphicsItem::ItemIsMovable,false);
}


void Game::removeCapturedPieces()
{
    for(auto i:capturedPieces)
    {
        int x=(i-1)/5;
        int y=((i-1)%5)*2+(x%2^1);
        board[x][y]->addPiece(PieceType::NONE,Color::NONE);
    }
    capturedPieces.clear();
}

Color Game::getTurn()
{
    return turn;
}

Game::Proxy Game::operator[](int index)
{
    return Proxy(board[index]);
}

void Game::sendMove(int from,int to)
{
    std::stringstream ss;
    if(conn->getplayerPlace()==2)
    {
        from=51-from;
        to=51-to;
    }
    ss << "ruch" << ' ' << from << ' ' << to;
    std::string msg=ss.str();
    conn->sendMove(msg);
}

void Game::makeMove(int from,int to)
{
    if(conn->getplayerPlace()==2)
    {
        from=51-from;
        to=51-to;
    }
    QPointF fromPos;
    QPointF toPos;
    Piece *piece=nullptr;
    for(int i=0;i<10;i++)
        for(int j=i%2^1;j<10;j+=2)
            if(board[i][j]->getIndex()==from)
            {
                fromPos=board[i][j]->getPosition();
                piece=board[i][j]->getPiece();
            }
            else if(board[i][j]->getIndex()==to)
                toPos=board[i][j]->getPosition();

    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    piece->setFlag(QGraphicsItem::ItemIsMovable);
    pressEvent.setScenePos(fromPos);
    pressEvent.setButton(Qt::LeftButton);
    pressEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(this, &pressEvent);

    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);
    releaseEvent.setScenePos(toPos);
    releaseEvent.setButton(Qt::LeftButton);
    releaseEvent.setButtons(Qt::LeftButton);
    QApplication::sendEvent(this, &releaseEvent);
    pressEvent.setButton(Qt::NoButton);
    pressEvent.setButtons(Qt::NoButton);
    QApplication::sendEvent(this, &pressEvent);
    piece->setFlag(QGraphicsItem::ItemIsMovable,false);
}

void Game::setConnection(Connection *conn)
{
    this->conn=conn;
}

Connection *Game::getConnection()
{
    return conn;
}


int Game::moveLength()
{
    for(int i=1;i<=50;i++)
    {
        Man::g[i].clear();
        King::g[i].clear();
    }
    for(int i=0;i<20;i++)
        moves[i].clear();
    for(int i=0;i<10;i++)
        for(int j=i%2^1;j<10;j+=2)
            if(board[i][j]->getPiece()->getColor()==Color::NONE)
                    board[i][j]->getPiece()->addEdges(i,j);
    int length=0;
    for(int i=0;i<10;i++)
        for(int j=i%2^1;j<10;j+=2)
            if(board[i][j]->getPiece()->getColor()==turn)
            {
                board[i][j]->getPiece()->addEdges(i,j);
                int pos=board[i][j]->getIndex();
                length=std::max(length,board[i][j]->getPiece()->moveLength(pos));
                board[i][j]->getPiece()->eraseEdges(i,j);
            }
    return length;
}

std::vector<std::pair<int,int>> Game::normalMoves()
{
    std::vector<std::pair<int,int>> v;
    for(int i=0;i<10;i++)
        for(int j=i%2^1;j<10;j+=2)
            if(board[i][j]->getPiece()->getColor()==turn)
                board[i][j]->getPiece()->addMoves(v,i,j);
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

void Game::changeTurn()
{
    turn = turn==Color::WHITE ? Color::BLACK : Color::WHITE;
    if(conn->getplayerPlace()==0) return;
    for(int i=0;i<10;i++)
        for(int j=i%2^1;j<10;j+=2)
            if(board[i][j]->getPiece()->getColor()==turn && turn==Color::WHITE)
                board[i][j]->getPiece()->setFlag(QGraphicsItem::ItemIsMovable);
            else
                board[i][j]->getPiece()->setFlag(QGraphicsItem::ItemIsMovable,false);
}


Game::Proxy::Proxy(Square **array)
    :array(array)
{

}

Square *Game::Proxy::operator[](int index)
{
    return array[index];
}

