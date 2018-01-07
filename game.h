#ifndef GAME_H
#define GAME_H

#include <QGraphicsScene>
#include <QtNetwork>
#include "connection.h"
#include "enum.h"
#include "square.h"

class Square;
class Connection;
class Game : public QGraphicsScene {
 private:
  Connection* conn;
  Square* board[10][10];
  Color turn;
  Color color;
  class Proxy {
   public:
    explicit Proxy(Square** array);
    Square* operator[](int index);

   private:
    Square** array;
  };

 public:
  Game();
  Color getTurn();
  void newGame(Color color);
  void startGame(Color color);
  void endGame();
  void changeTurn();
  int moveLength();
  void removeCapturedPieces();
  std::vector<std::vector<std::tuple<int, int, int>>> possibleMoves(int length);
  std::vector<std::pair<int, int>> normalMoves();
  std::vector<int> capturedPieces;
  std::vector<std::tuple<int, int, int>> moves[20];
  Proxy operator[](int index);
  void sendMove(int from, int to);
  void makeMove(int from, int to);
  void setConnection(Connection* conn);
  Connection* getConnection();
  Color getColor() const;
};

#endif  // GAME_H
