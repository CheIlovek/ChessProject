#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h" // must be included
#include "ChessGrid.h"
#include "GameMode.h"


ChessGrid::ChessGrid(unsigned int width, unsigned int height) :
	chessGrid(width * height),
	width(width), height(height) {
}

ChessGrid::~ChessGrid() {
	clear();
}

ChessGrid::ChessGrid(ChessGrid& other) {
	width = other.width;
	height = other.height;
	chessGrid.resize(other.chessGrid.size());
	for (int i = 0; i < chessGrid.size(); i++) {
		if (!other.chessGrid[i]) continue;
		chessGrid[i].reset(new Piece(*other.chessGrid[i]));
	}
	
}

void ChessGrid::clear() {
	chessGrid.clear();
	chessGrid.resize(width * height);
}

bool ChessGrid::movePiece(short from, short to) {
	size_t size = chessGrid.size();
	if (from < 0 || from >= size || to < 0 || to >= size) {
		spdlog::warn("Attempt to access memory beyond grid. From: {}, To: {}, Size: {}", from, to, chessGrid.size());
		return false;
	}
	
	chessGrid[to].reset(chessGrid[from].release());
	return true;
	
}

bool ChessGrid::movePiece(Point from, Point to) {
	return movePiece(from.col + from.row * width, to.col + to.row * width);
}

void ChessGrid::removePiece(Point point) {
	removePiece(point.col + point.row * width);
}

void ChessGrid::removePiece(int row, int col) {
	removePiece(col + row * width);
}

void ChessGrid::removePiece(int position) {
	if (position < 0 || position >= chessGrid.size()) {
		spdlog::warn("Attempt to access memory beyond grid. Position: {}, Size: {}", position, chessGrid.size() );
		return;
	}
	chessGrid[position].reset();
}

void ChessGrid::setPiece(Point point, PiecesTypes type, Teams team) {
	if (point.row < 0 || point.row >= height || point.col < 0 || point.col >= width) {
		spdlog::warn("Attempt to access memory beyond grid. Row: {}, Column: {}, Size: {}", point.row, point.col, chessGrid.size());
		return;
	}
	int pos = point.col + point.row * width;
	setPiece(pos, type, team);
}

void ChessGrid::setPiece(int row, int col, PiecesTypes type, Teams team) {
	if (row < 0 || row >= height || col < 0 || col >= width) {
		spdlog::warn("Attempt to access memory beyond grid. Row: {}, Column: {}, Size: {}", row, col, chessGrid.size());
		return;
	}
	int pos = col + row * width;
	setPiece(pos, type, team);
}

void ChessGrid::setPiece(int position, PiecesTypes type, Teams team) {
	if (position < 0 || position >= chessGrid.size()) {
		spdlog::warn("Attempt to access memory beyond grid. Position: {}, Size: {}", position, chessGrid.size());
		return;
	}
	chessGrid[position].reset(new Piece(type, team));
}

void ChessGrid::setPiece(Point point, Piece* piece) {
	int pos = point.col + point.row * width;
	if (pos < 0 || pos >= chessGrid.size()) {
		spdlog::warn("Attempt to access memory beyond grid. Position: {}, Size: {}", point, chessGrid.size());
		return;
	}
	chessGrid[pos].reset(piece);
}

// »щет позицию фигуры по типу и команде, в случае неудачи - возвращает Point(-1,-1).
Point ChessGrid::find(PiecesTypes type, Teams team) {
	for (int i = 0; i < chessGrid.size(); i++) {
		const Piece* piece = chessGrid[i].get();
		if (!piece) continue;
		if (piece->type == type && piece->team == team) {
			return Point(i / width, i % width);
		}
	}
	return Point(-1, -1);
}

Piece* ChessGrid::operator[](int index) {
	return chessGrid[index].get();
}

Piece* ChessGrid::operator[](Point point) {
	int index = point.row * width + point.col;
	return chessGrid[index].get();
}

Piece* ChessGrid::getOrDefault(int index, Piece* defaultValue) {
	if (index < 0 || index >= chessGrid.size()) {
		return defaultValue;
	}
	return (*this)[index];
	
}

Piece* ChessGrid::getOrDefault(Point point, Piece* defaultValue) {
	if (point.row < 0 || point.row >= height || point.col < 0 || point.col >= width) {
		return defaultValue;
	}
	int index = point.row * width + point.col;
	return (*this)[index];
}





