#pragma once
#include "ChessEnums.h"

struct Piece {
	PiecesTypes type;
	Teams team;

	Piece() : type(PiecesTypes::NONE), team(Teams::NEUTRAL) {};
	Piece(const PiecesTypes type, const Teams team)
		: type(type), team(team) {
	}
	Piece(Piece& piece) : type(piece.type), team(piece.team) {
	}


};