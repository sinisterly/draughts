#ifndef MOVE_H
#define MOVE_H

#include <vector>

class Move
{
private:
    int from;
    int to;
    std::vector <int> v;
public:
    Move();
    Move(int from,int to);
};

#endif // MOVE_H
