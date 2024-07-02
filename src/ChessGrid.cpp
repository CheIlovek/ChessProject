#include "ChessGrid.h"
#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h" // must be included
#include "Point.h"
#include "Piece.h"
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
		chessGrid[i] = Piece::getObjectByTypeAndTeam(other.chessGrid[i]->team, other.chessGrid[i]->type);
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

Teams ChessGrid::getTeamOnSquareOrDefault(Point point, Teams defaultTeam) {
	if (point.row < 0 || point.row >= height || point.col < 0 || point.col >= width) {
		spdlog::warn("Attempt to access memory beyond grid. Row: {}, Column: {}, Size: {}", point.row, point.col, chessGrid.size());
		return defaultTeam;
	}
	int index = point.row * width + point.col;
	Piece* piece = chessGrid[index].get();
	if (!piece) {
		return defaultTeam;
	}
	return piece->team;
}

Teams ChessGrid::getTeamOnSquare(Point point) {
	int index = point.row * width + point.col;
	Piece* piece = chessGrid[index].get();
	if (!piece) {
		return Teams::ENEMY_TO_ALL;
	}
	return piece->team;
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

void ChessGrid::setPiece(Point point, Piece* piece) {
	int pos = point.col + point.row * width;
	if (pos < 0 || pos >= chessGrid.size()) {
		spdlog::warn("Attempt to access memory beyond grid. Position: {}, Size: {}", point, chessGrid.size());
		return;
	}
	chessGrid[pos].reset(piece);
}

Point ChessGrid::find(Teams team, PiecesTypes type) {
	for (int i = 0; i < chessGrid.size(); i++) {
		Piece* curPiece = chessGrid[i].get();
		if (curPiece && curPiece->team == team && curPiece->type == type) {
			return { (short) (i / width), (short) (i % width) };
		}
	}
	return { -1, -1 };
}


Piece* ChessGrid::operator[](int index) {
	return chessGrid[index].get();
}

Piece* ChessGrid::operator[](Point point) {
	int index = point.row * width + point.col;
	return chessGrid[index].get();
}

Piece* ChessGrid::pickUpPiece(Point point) {
	if (point.row < 0 || point.row >= height || point.col < 0 || point.col >= width) {
		return nullptr;
	}
	pickUpPiece(point.col + point.row * width);
}

Piece* ChessGrid::pickUpPiece(int index) {
	if (index < 0 || index >= chessGrid.size()) {
		return nullptr;
	}
	return chessGrid[index].release();
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

bool ChessGrid::isValidPoint(Point point) const {
	return point.row >= 0 && point.row < height && point.col >= 0 && point.col < width;
}





