#pragma once
#include "ChessGrid.h"
#include "Point.h"


class GameMode {
public:
	// TODO: ��������, ������� ��� ������ ����� ����� (��� ������), � ��� ��������� ��� ������������ �������

	GameMode();
	ChessGrid* getGrid();
	

	void resetBoard(); // ���������� �� ��� �����
	bool putPiece	(Point gridCoords);
	void pickUpPiece(Point gridCoords);
	const Piece* getPickedUpPiece();
	std::vector<short> getAllPossibleMoves(short pieceId);
	
private:
	std::vector<Point> getAllPossibleMovesPawn		(Point position);
	std::vector<Point> getAllPossibleMovesKnight	(Point position);
	std::vector<Point> getAllPossibleMovesBishop	(Point position);
	std::vector<Point> getAllPossibleMovesRook		(Point position);
	std::vector<Point> getAllPossibleMovesQueen		(Point position);
	std::vector<Point> getAllPossibleMovesKing		(Point position);

	// �������� �� ��������� ������ ������� team �� position? 
	// �� ��������� ������������ ��������� ������.
	bool isCorrectMove(const Point& position, Teams team);

	// ��������� �� ������ ��� �����?
	bool isKingUnderAttack	(Teams team);

	// ����� �� ����� ��������� ���?
	bool haveAvailableMoves	(Teams team);

	// ������� ����������� ����� �������
	void placeTeamPieces(const Teams team, const short row, const short direction);


	// ��������� �����
	ChessGrid grid;

	// ID ����� ������� ����� ����� �� ������� "������ �� �������"
	// ������ ���������� �� ��������� ����
	std::unique_ptr<Point> enPassantPawn;
	Teams teamToMove = Teams::WHITE;
	GameStates state = GameStates::FREEPLAY;
	std::unique_ptr<Piece> pickedUpPiece;
	std::vector<Point> pickedUpPieceAvailableMoves;

	const short startRowForWhitePawns = 6;
	const short startRowForBlackPawns = 1;

};

struct Piece {
	PiecesTypes type;
	Teams team;

	Piece(const PiecesTypes type, const Teams team)
		: type(type), team(team) {
	}
};



