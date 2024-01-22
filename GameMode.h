#pragma once
#include "ChessGrid.h"
#include "Point.h"


class GameMode {
public:
	// TODO: Возможно, сделать что всегда белые снизу (или сверху), и при отрисовке уже поворачивать позицию

	GameMode();
	ChessGrid* getGrid();
	

	void resetBoard(); // Расставить всё как надоо
	bool putPiece	(Point gridCoords);
	void pickUpPiece(Point gridCoords);
	const Piece* getPickedUpPiece();
	std::vector<short> getAllPossibleMoves(short pieceId);
	
private:
	std::vector<Point> getAllPossibleMovesPawn		(Point position);
	std::vector<Point> getAllPossibleMovesKnight	(Point position);
	std::vector<Point> getAllPossibleMovesBishop	(Point position);
	std::vector<Point> getAllPossibleMovesRook		(Point position);
	std::vector<Point> getAllPossibleMovesQueen		(Point position);
	std::vector<Point> getAllPossibleMovesKing		(Point position);

	// Возможно ли поставить фигуру команды team на position? 
	// Не учитывает изначального положения фигуры.
	bool isCorrectMove(const Point& position, Teams team);

	// Находится ли король под шахом?
	bool isKingUnderAttack	(Teams team);

	// Может ли игрок совершить ход?
	bool haveAvailableMoves	(Teams team);

	// Базовая расстоновка фигур команды
	void placeTeamPieces(const Teams team, const short row, const short direction);


	// Шахматная доска
	ChessGrid grid;

	// ID пешки которую можно взять по правилу "Взятие на проходе"
	// Должно обнуляться на следующем ходу
	std::unique_ptr<Point> enPassantPawn;
	Teams teamToMove = Teams::WHITE;
	GameStates state = GameStates::FREEPLAY;
	std::unique_ptr<Piece> pickedUpPiece;
	std::vector<Point> pickedUpPieceAvailableMoves;

	const short startRowForWhitePawns = 6;
	const short startRowForBlackPawns = 1;

};

struct Piece {
	PiecesTypes type;
	Teams team;

	Piece(const PiecesTypes type, const Teams team)
		: type(type), team(team) {
	}
};



