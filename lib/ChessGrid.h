#pragma once
#include <vector>
#include <memory>
#include "ChessEnums.h"
#include "Point.h"
#include "Piece.h"


// TODO: Заменить на юник птр-ы
// Класс шахматной доски.
class ChessGrid {
public:
	ChessGrid(unsigned int width, unsigned int height);
	~ChessGrid();
	ChessGrid(ChessGrid& other);
	

	auto begin() {
		return chessGrid.begin();
	}
	auto end() {
		return chessGrid.end();
	}

	void clear();
	bool movePiece(short from, short to);
	bool movePiece(Point from, Point to);

	Teams getTeamOnSquareOrDefault(Point point, Teams defaultTeam);
	Teams getTeamOnSquare(Point point);

	void removePiece(Point point);
	void removePiece(int row, int col);
	void removePiece(int position);

	void setPiece(Point point,		Piece* piece);

	Point find(Teams team, PiecesTypes type);

	Piece* operator[](int index);
	Piece* operator[](Point point);


	Piece* pickUpPiece(int index);
	Piece* pickUpPiece(Point point);

	Piece* getOrDefault(int index,			Piece* defaultValue);
	Piece* getOrDefault(Point point,		Piece* defaultValue);

	bool isValidPoint(Point point) const;


private:
	std::vector<std::unique_ptr<Piece>> chessGrid;

	unsigned int width;
	unsigned int height;
};

