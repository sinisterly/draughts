#include "game.h"
#include "enum.h"


Game::Game(QGraphicsScene *scene)
    :scene(scene){
    newGame();
    //loop()
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



int Game::moveLength()
{
    int length=0;
    for(int i=0;i<10;i++)
        for(int j=0;j<10;j++)
            if(board[i][j]->getPiece()->getColor()==turn)
                length=std::max(length,moveLength(board[i][j]->getIndex()));
    return length;
}

int Game::moveLength(int pos)
{
    int length=0;

    return length;
}

void Game::loop()
{
    newGame();
    while(1)
    {

    }
}

