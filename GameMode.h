#pragma once
#include "ChessGrid.h"
#include "Point.h"


class GameMode {
public:
	// TODO: Возможно, сделать что всегда белые снизу (или сверху), и при отрисовке уже поворачивать позицию

	GameMode();
	ChessGrid* getGrid();
	
	void squareInteraction(Point gridCoords);
	void resetBoard(); // Расставить всё как надоо
	bool makeMove	(Point gridCoords);
	void pickUpPiece(Point gridCoords);
	const Piece* getPickedUpPiece();
	const std::vector<Point> getAllPossibleMovesForPickedUpPiece() const;
	
private:

	
	//TODO Рокировка

	std::vector<Point> getAllPossibleMovesPawn		(Point position, ChessGrid& gridToTest);
	std::vector<Point> getAllPossibleMovesKnight	(Point position, ChessGrid& gridToTest);
	std::vector<Point> getAllPossibleMovesBishop	(Point position, ChessGrid& gridToTest);
	std::vector<Point> getAllPossibleMovesRook		(Point position, ChessGrid& gridToTest);
	std::vector<Point> getAllPossibleMovesQueen		(Point position, ChessGrid& gridToTest);
	std::vector<Point> getAllPossibleMovesKing		(Point position, ChessGrid& gridToTest);

	bool isMoveSafe(Teams team, Point prevPos, Point nextPos, ChessGrid& gridToTest);
	bool couldThreatTheKingByMove(Teams team, Point piecePos, ChessGrid& gridToTest);

	// Возможен ли ход. Оценивается по:
	// 1. Не выходит за границы доски.
	// 2. Не подставляет короля под атаку. (опционально)
	bool isCorrectMove(const Point curPos, const Point nextPos, Teams team, ChessGrid& gridToTest, bool checkKingSafety = true);

	// Находится ли король под шахом?
	bool isKingUnderAttack	(Teams team, ChessGrid& gridToTest);

	// Может ли игрок совершить ход?
	bool haveAvailableMoves	(Teams team, ChessGrid& gridToTest);

	// Базовая расстоновка фигур команды
	void placeTeamPieces(const Teams team, const short row, const short direction);


	struct PickedUpPiece {
		std::unique_ptr<Piece> piece;
		std::vector<Point> pieceAvailableMoves;
		Point piecePreviousPosition;

		PickedUpPiece() : piecePreviousPosition(-1,-1) {};

		// Возвращает указатель на Piece, который необходимо обработать
		Piece* clear() {
			pieceAvailableMoves.clear();
			piecePreviousPosition = Point(-1, -1);
			return piece.release();
		}

		void init(Piece* piece, std::vector<Point> availableMoves, Point position) {
			pieceAvailableMoves = availableMoves;
			piecePreviousPosition = position;
			this->piece.reset(new Piece(*piece));
		}

	};

	// Шахматная доска
	ChessGrid mainGrid;

	// ID пешки которую можно взять по правилу "Взятие на проходе"
	// Должно обнуляться на следующем ходу
	std::unique_ptr<Point> enPassantPawn;
	Teams teamToMove = Teams::WHITE;
	GameStates state = GameStates::FREEPLAY;
	PickedUpPiece pickedUpPiece;

	const short startRowForWhitePawns = 6;
	const short startRowForBlackPawns = 1;

	

};







