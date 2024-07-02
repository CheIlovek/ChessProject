#pragma once
#include <iostream>
#include <vector>
#include "ChessEnums.h"
#include "Point.h"

class ChessGrid;

class Piece {

public:
	const Teams team;
	const PiecesTypes type;

	Piece(const Teams team, const PiecesTypes type)
		:  team(team), type(type) {
	}
	Piece(Piece& piece) :  team(piece.team), type(piece.type) {
	}

	virtual ~Piece() {};

	virtual std::vector<Point> getAllPossibleMoves(Point position, ChessGrid& gridToTest) = 0;
	static std::unique_ptr<Piece> getObjectByTypeAndTeam(Teams team, PiecesTypes type, short moveDirection = 1);


	// TODO: для каждого класса свой
	friend std::ostream& operator<<(std::ostream& os, const Piece& p);

};

class Knight : public Piece {

public:
	Knight(Teams team) : Piece(team,PiecesTypes::KNIGHT) {};
	~Knight() = default;

	std::vector<Point> getAllPossibleMoves(Point position, ChessGrid& gridToTest);
};

class King : public Piece {

public:
	King(Teams team) : Piece(team, PiecesTypes::KING) {};
	~King() = default;

	std::vector<Point> getAllPossibleMoves(Point position, ChessGrid& gridToTest);
};


class Pawn : public Piece {
	
public:
	Pawn(const Teams team, short moveDirection) : Piece(team, PiecesTypes::PAWN) {
		this->moveDirection = moveDirection;
	};
	~Pawn() = default;

	std::vector<Point> getAllPossibleMoves(Point position, ChessGrid& gridToTest);

private:
	short moveDirection = 1;
};


class Rook : public Piece {

public:
	Rook(Teams team) : Piece(team, PiecesTypes::ROOK) {};
	~Rook() = default;

	std::vector<Point> getAllPossibleMoves(Point position, ChessGrid& gridToTest);

};

class Bishop : public Piece {

public:
	Bishop(Teams team) : Piece(team, PiecesTypes::BISHOP) {};
	~Bishop() = default;

	std::vector<Point> getAllPossibleMoves(Point position, ChessGrid& gridToTest);
};

class Queen : public Piece {

public:
	Queen(Teams team) : Piece(team, PiecesTypes::QUEEN) {};
	~Queen() = default;

	std::vector<Point> getAllPossibleMoves(Point position, ChessGrid& gridToTest);

};