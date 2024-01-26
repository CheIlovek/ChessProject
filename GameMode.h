#pragma once
#include "ChessGrid.h"
#include "Point.h"


class GameMode {
public:
	// TODO: ��������, ������� ��� ������ ����� ����� (��� ������), � ��� ��������� ��� ������������ �������

	GameMode();
	ChessGrid* getGrid();
	
	void squareInteraction(Point gridCoords);
	void resetBoard(); // ���������� �� ��� �����
	bool makeMove	(Point gridCoords);
	void pickUpPiece(Point gridCoords);
	const Piece* getPickedUpPiece();
	const std::vector<Point> getAllPossibleMovesForPickedUpPiece() const;
	
private:

	
	//TODO ���������

	std::vector<Point> getAllPossibleMovesPawn		(Point position, ChessGrid& gridToTest);
	std::vector<Point> getAllPossibleMovesKnight	(Point position, ChessGrid& gridToTest);
	std::vector<Point> getAllPossibleMovesBishop	(Point position, ChessGrid& gridToTest);
	std::vector<Point> getAllPossibleMovesRook		(Point position, ChessGrid& gridToTest);
	std::vector<Point> getAllPossibleMovesQueen		(Point position, ChessGrid& gridToTest);
	std::vector<Point> getAllPossibleMovesKing		(Point position, ChessGrid& gridToTest);

	bool isMoveSafe(Teams team, Point prevPos, Point nextPos, ChessGrid& gridToTest);
	bool couldThreatTheKingByMove(Teams team, Point piecePos, ChessGrid& gridToTest);

	// �������� �� ���. ����������� ��:
	// 1. �� ������� �� ������� �����.
	// 2. �� ����������� ������ ��� �����. (�����������)
	bool isCorrectMove(const Point curPos, const Point nextPos, Teams team, ChessGrid& gridToTest, bool checkKingSafety = true);

	// ��������� �� ������ ��� �����?
	bool isKingUnderAttack	(Teams team, ChessGrid& gridToTest);

	// ����� �� ����� ��������� ���?
	bool haveAvailableMoves	(Teams team, ChessGrid& gridToTest);

	// ������� ����������� ����� �������
	void placeTeamPieces(const Teams team, const short row, const short direction);


	struct PickedUpPiece {
		std::unique_ptr<Piece> piece;
		std::vector<Point> pieceAvailableMoves;
		Point piecePreviousPosition;

		PickedUpPiece() : piecePreviousPosition(-1,-1) {};

		// ���������� ��������� �� Piece, ������� ���������� ����������
		Piece* clear() {
			pieceAvailableMoves.clear();
			piecePreviousPosition = Point(-1, -1);
			return piece.release();
		}

		void init(Piece* piece, std::vector<Point> availableMoves, Point position) {
			pieceAvailableMoves = availableMoves;
			piecePreviousPosition = position;
			this->piece.reset(new Piece(*piece));
		}

	};

	// ��������� �����
	ChessGrid mainGrid;

	// ID ����� ������� ����� ����� �� ������� "������ �� �������"
	// ������ ���������� �� ��������� ����
	std::unique_ptr<Point> enPassantPawn;
	Teams teamToMove = Teams::WHITE;
	GameStates state = GameStates::FREEPLAY;
	PickedUpPiece pickedUpPiece;

	const short startRowForWhitePawns = 6;
	const short startRowForBlackPawns = 1;

	

};







