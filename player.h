#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "move.h"


class Player
{
public:
    Player();
    std::vector<Move> getPossibleMoves();
};

#endif // PLAYER_H
