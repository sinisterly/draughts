#include "emptypiece.h"
#include "man.h"
#include "king.h"

EmptyPiece::EmptyPiece(Color color)
    :Piece(color)
{

}

void EmptyPiece::addEdges(int x, int y)
{
    Man* man=new Man(Color::NONE);
    man->addEdges(x,y);
    delete man;
    King* king=new King(Color::NONE);
    king->addEdges(x,y);
    delete king;
}

