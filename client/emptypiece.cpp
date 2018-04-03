#include "emptypiece.h"
#include "king.h"
#include "man.h"

EmptyPiece::EmptyPiece(Color color) : Piece(color) {}

void EmptyPiece::addEdges(int x, int y) {
  Man* man = new Man(Color::NONE);
  man->addEdges(x, y);
  delete man;
  King* king = new King(Color::NONE);
  king->addEdges(x, y);
  delete king;
}
