#pragma once

enum Teams {
	ENEMY_TO_ALL,
	WHITE,
	BLACK,
};


// NONE ������ ���������� ���������!!
enum PiecesTypes {
	KING,
	QUEEN,
	ROOK,
	BISHOP,
	KNIGHT,
	PAWN,
	NONE,
};

enum GameStates {
	FREEPLAY,
	CHECK,
	CHECKMATE,
};