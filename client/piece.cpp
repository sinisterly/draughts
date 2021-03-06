#include "piece.h"
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include "square.h"

int Piece::vis[51];

QRectF Piece::boundingRect() const { return QRectF(0, 0, 40, 40); }

void Piece::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *) {
  QPen pen(Qt::black, 3);
  QBrush brush(Qt::red);
  painter->setPen(pen);
  if (color == Color::WHITE)
    painter->setBrush(game->getColor() == Color::WHITE ? Qt::white : Qt::red);
  if (color == Color::BLACK)
    painter->setBrush(game->getColor() == Color::WHITE ? Qt::red : Qt::white);
  paint(painter);
}

Color Piece::getColor() { return color; }

void Piece::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  parentItem()->setZValue(1);
  update();
  QGraphicsItem::mousePressEvent(event);
}

void Piece::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  static bool isCapturing;
  static int totalCaptureLength;
  static int currentCaptureLength;
  Square *parent = qgraphicsitem_cast<Square *>(parentItem());
  parent->setZValue(0);
  int from = parent->getIndex();
  Square *newParent = nullptr;
  foreach (QGraphicsItem *item, scene()->items(mapToScene(event->pos()))) {
    Square *node = qgraphicsitem_cast<Square *>(item);
    if (!node || node == qgraphicsitem_cast<Square *>(this)) continue;
    newParent = node;
  }
  if (newParent == nullptr) {
    setPos(5, 5);
    return;
  }
  int to = newParent->getIndex();
  if (!(isCapturing)) {
    int length = game->moveLength();
    if (length == 0) {
      auto moves = game->normalMoves();
      bool okMove = std::find(moves.begin(), moves.end(),
                              std::make_pair(from, to)) != moves.end();
      if (okMove) {
        Piece *piece2 = newParent->getPiece();
        piece2->setParentItem(parent);
        setParentItem(newParent);
        parent->updatePiece();
        newParent->updatePiece();
        if ((color == Color::WHITE && to <= 5) ||
            (color == Color::BLACK && to >= 46)) {
          newParent->addPiece(PieceType::KING, color);
        }
        if (game->getConnection()->getplayerPlace() != 0 &&
            game->getTurn() == Color::WHITE)
          game->sendMove(from, to);
        game->changeTurn();
      }
    } else {
      totalCaptureLength = length;
      currentCaptureLength = 0;
      isCapturing = true;
    }
  }
  if (isCapturing) {
    auto moves = game->possibleMoves(totalCaptureLength);
    auto it = std::find_if(
        moves[currentCaptureLength].begin(), moves[currentCaptureLength].end(),
        [&](std::tuple<int, int, int> &x) {
          return std::get<0>(x) == from && std::get<1>(x) == to;
        });
    bool okMove = (it != moves[currentCaptureLength].end());
    if (okMove) {
      Piece *piece2 = newParent->getPiece();
      piece2->setParentItem(parent);
      setParentItem(newParent);
      parent->updatePiece();
      newParent->updatePiece();
      currentCaptureLength++;
      game->capturedPieces.push_back(std::get<2>(*it));
      if (game->getConnection()->getplayerPlace() != 0 &&
          game->getTurn() == Color::WHITE)
        game->sendMove(from, to);
      if (totalCaptureLength == currentCaptureLength) {
        isCapturing = false;
        game->removeCapturedPieces();
        if ((color == Color::WHITE && to <= 5) ||
            (color == Color::BLACK && to >= 46)) {
          newParent->addPiece(PieceType::KING, color);
        }
        game->changeTurn();
      }
    }
  }
  setPos(5, 5);
  update();

  if (game->getConnection()->getplayerPlace() != 0 &&
      game->getTurn() == Color::BLACK && !isCapturing &&
      game->moveLength() == 0 && game->normalMoves().size() == 0) {
    game->getConnection()->sendMessage("win");
  }

  QGraphicsItem::mouseReleaseEvent(event);
}

Piece::Piece(Color color) : color(color) {}
