#pragma once

enum Teams {
	NEUTRAL,
	WHITE,
	BLACK,
};

enum PiecesTypes {
	NONE,
	KING,
	QUEEN,
	ROOK,
	BISHOP,
	KNIGHT,
	PAWN,
};

enum GameStates {
	FREEPLAY,
	CHECK,
	CHECKMATE,
};