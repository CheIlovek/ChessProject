#pragma once
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "ChessEnums.h"
#include "Point.h"


struct Piece;
// TODO: Заменить на юник птр-ы
// Класс шахматной доски.
class ChessGrid {
public:
	ChessGrid();
	~ChessGrid();
	

	auto begin() {
		return chessGrid.begin();
	}
	auto end() {
		return chessGrid.end();
	}

	void clear();
	bool movePiece(short from, short to);

	Piece*& operator[](int i);
	Piece*& operator[](Point point);
	Piece*& operator[](sf::Vector2i point);

	Point find(PiecesTypes type, Teams team);

	Piece* getOrDefault(int index,			Piece* defaultValue);
	Piece* getOrDefault(Point point,		Piece* defaultValue);
	Piece* getOrDefault(sf::Vector2i point, Piece* defaultValue);


private:
	std::vector<Piece*> chessGrid;
};

