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

	friend std::ostream& operator<<(std::ostream& os, const Piece& p) {
		std::string teamTitle, typeTitle;
		switch (p.team) {
		case Teams::BLACK:
			teamTitle = "Black";
			break;
		case Teams::WHITE:
			teamTitle = "White";
			break;
		case Teams::NEUTRAL:
			teamTitle = "Neutral";
			break;
		}

		switch (p.type) {
		case PiecesTypes::BISHOP:
			typeTitle = "Bishop";
			break;
		case PiecesTypes::KING:
			typeTitle = "King";
			break;
		case PiecesTypes::KNIGHT:
			typeTitle = "Knight";
			break;
		case PiecesTypes::NONE:
			typeTitle = "None";
			break;
		case PiecesTypes::PAWN:
			typeTitle = "Pawn";
			break;
		case PiecesTypes::QUEEN:
			typeTitle = "Queen";
			break;
		case PiecesTypes::ROOK:
			typeTitle = "Rook";
			break;
		}

		return os << "(Team: " << teamTitle << ", Type: " << typeTitle << ")";
	}

};