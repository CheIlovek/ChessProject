#include "ChessGrid.h"
#include "GameMode.h"


ChessGrid::ChessGrid() :
	chessGrid(64, nullptr) {
}

ChessGrid::~ChessGrid() {
	clear();
}

void ChessGrid::clear() {
	for (int i = 0; i < chessGrid.size(); i++) {
		delete chessGrid[i];
	}
	chessGrid.clear();
	chessGrid.resize(64, nullptr);
}

bool ChessGrid::movePiece(short from, short to) {
	if (from < 0 && from >= 64 && to < 0 && to >= 64) {
		return false;
	}
	if (chessGrid[to] != nullptr) {
		delete chessGrid[to];
	}
	chessGrid[to] = chessGrid[from];
	chessGrid[from] = nullptr;
	
	return true;
	
}
//TODO должно быть чёто другое
//TODO: Возвращать nullptr при выходе за рамки?
Piece*& ChessGrid::operator[](int i) {
	return chessGrid[i];
}

Piece*& ChessGrid::operator[](Point point) {
	return chessGrid[point.row * 8 + point.col];
}

Piece*& ChessGrid::operator[](sf::Vector2i point) {
	return chessGrid[point.y * 8 + point.x];
}

// Ищет позицию фигуры по типу и команде, в случае неудачи - возвращает Point(-1,-1).
Point ChessGrid::find(PiecesTypes type, Teams team) {
	for (int i = 0; i < chessGrid.size(); i++) {
		const Piece* piece = chessGrid[i];
		if (!piece) continue;
		if (piece->type == type && piece->team == team) {
			return Point(i / 8, i % 8);
		}
	}
	return Point(-1, -1);
}

Piece* ChessGrid::getOrDefault(int index, Piece* defaultValue) {
	if (index < 0 && index >= chessGrid.size()) {
		return defaultValue;
	}
	return (*this)[index];
	
}

Piece* ChessGrid::getOrDefault(Point point, Piece* defaultValue) {
	int index = point.row * 8 + point.col;
	if (index < 0 && index >= chessGrid.size()) {
		return defaultValue;
	}
	return (*this)[index];
}

Piece* ChessGrid::getOrDefault(sf::Vector2i point, Piece* defaultValue) {
	int index = point.y * 8 + point.x;
	if (index < 0 && index >= chessGrid.size()) {
		return defaultValue;
	}
	return (*this)[index];
}




