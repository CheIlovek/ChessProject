#include <iostream>
#include "GameMode.h"
#include "Piece.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h" // must be included

GameMode::GameMode() : mainGrid(8,8) {
}

ChessGrid* GameMode::getGrid() {
	return &mainGrid;
}

void GameMode::squareInteraction(Point gridCoords) {

	// ������� ������ �� �����
	if (gridCoords == pickedUpPiece.piecePreviousPosition) {
		mainGrid.setPiece(
			pickedUpPiece.piecePreviousPosition, 
			pickedUpPiece.piece.release());
		spdlog::info("Figure is placed back");
		pickedUpPiece.clear();
		return;
	}

	if (!pickedUpPiece.piece) { // ���� ������ �� ����� - ���������� ����� � ����� ��������
		pickUpPiece(gridCoords);
	} else {					// ����� - ��������� ������ �� ��� �����.
		makeMove(gridCoords);
	}
}

void GameMode::resetBoard() {
	mainGrid.clear();
	placeTeamPieces(Teams::WHITE, 7, -1);
	placeTeamPieces(Teams::BLACK, 0, 1);

	spdlog::info("Board is reseted");
}

bool GameMode::makeMove(Point gridCoords) {

	// ��� ���������
	std::vector<Point>& availableMoves = pickedUpPiece.pieceAvailableMoves;
	if (std::find(availableMoves.begin(), availableMoves.end(), gridCoords) == availableMoves.end()) {
		spdlog::debug("Tried to make illegal move");
		return false;
	}

	// ���� ��� ������ �� ������� - �� ����� ������� ������ enPassantPawn
	short direction = pickedUpPiece.piece->team == Teams::WHITE ? -1 : 1;
	if (enPassantPawn && pickedUpPiece.piece->type == PiecesTypes::PAWN && gridCoords.col == enPassantPawn->col && enPassantPawn->row + direction == gridCoords.row) {
		mainGrid.removePiece(*enPassantPawn);
	}


	// ��� ������ �� 2 �����
	if (pickedUpPiece.piece->type == PiecesTypes::PAWN && std::abs(pickedUpPiece.piecePreviousPosition.row - gridCoords.row) == 2) {
		enPassantPawn.reset(new Point(gridCoords));
	} else {
		enPassantPawn.reset();
	}

	mainGrid.setPiece(gridCoords, pickedUpPiece.clear());
	spdlog::info("Piece is placed");

	// �������� ��� �� ���, ��� ���
	teamToMove = teamToMove == Teams::WHITE ? Teams::BLACK : Teams::WHITE;
	if (isKingUnderAttack(teamToMove, mainGrid)) {
		if (haveAvailableMoves(teamToMove, mainGrid)) {
			state = GameStates::CHECK;
			spdlog::info("CHECK on board!");
		} else {
			state = GameStates::CHECKMATE;
			spdlog::info("CHECKMATE on board!");
		}
	}
	return true;
}

void GameMode::pickUpPiece(Point gridCoords) {
	Piece* piece = mainGrid.getOrDefault(gridCoords,nullptr);
	if (piece == nullptr) {
		spdlog::debug("Tried to pick up empty square");
		return;
	}

	if (piece->team != teamToMove) {
		//TODO
		spdlog::debug("Tried to make move before HIS turn");
		return;
	}
	mainGrid.pickUpPiece(gridCoords);
	std::vector<Point> pickedUpPieceAvailableMoves = piece->getAllPossibleMoves(gridCoords, mainGrid);
	pickedUpPiece.init(piece, pickedUpPieceAvailableMoves, gridCoords);
	spdlog::info("Piece picked up");
}

const Piece* GameMode::getPickedUpPiece() {
	return pickedUpPiece.piece.get();
}

const std::vector<Point> GameMode::getAllPossibleMovesForPickedUpPiece() const {
	return pickedUpPiece.pieceAvailableMoves;
}

void GameMode::checkForSpecialRules(Point piecePos, PiecesTypes type, ChessGrid& gridToTest) {
	// ������ ��� ����� �� 2 ��.

}


bool GameMode::isMoveSafe(Teams team, Point prevPos, Point nextPos, ChessGrid& gridToTest) {
	ChessGrid modifiedGrid(gridToTest);
	modifiedGrid.movePiece(prevPos, nextPos);
	return !isKingUnderAttack(team, modifiedGrid);
}

bool GameMode::couldThreatTheKingByMove(Teams team, Point piecePos, ChessGrid& gridToTest) {
	ChessGrid modifiedGrid(gridToTest);
	modifiedGrid.removePiece(piecePos);
	return isKingUnderAttack(team, modifiedGrid);;
}


bool GameMode::isCorrectMove(const Point curPos, const Point nextPos, Teams team, ChessGrid& gridToTest, bool checkKingSafety) {
	//TODO wtf
	if (!(gridToTest.getTeamOnSquareOrDefault(nextPos, team) != team)) {
		return false;
	}

	return !checkKingSafety || isMoveSafe(team, curPos, nextPos, gridToTest);
}

bool GameMode::isKingUnderAttack(Teams team, ChessGrid& gridToTest) {
	// ���� ������
	Point kingLocation = gridToTest.find(team, PiecesTypes::KING);
	if (kingLocation == Point(-1,-1)) {
		spdlog::error("King is missing!");
		//TODO: ERROR (������ �����?!)
	}

	return isSquareUnderAttack(kingLocation, team, gridToTest);
}

bool GameMode::isSquareUnderAttack(Point square, Teams team, ChessGrid& gridToTest) {

	// �������� ������������� ��� ������ ������:
	// ����� ������ �������. (����������: "SpecialRules", ��� ����� ������������).
	// ���������: ���� ������ �1 ���� � ����� ������ ������ �2 ���� �, �� � ������ �2 ����� ������ ������ �1. 


	for (int typeInt = 0; typeInt != PiecesTypes::NONE; typeInt++) {
		PiecesTypes curType = static_cast<PiecesTypes>(typeInt);

		auto piece = Piece::getObjectByTypeAndTeam(team, curType);
		std::vector<Point> allMoves = piece->getAllPossibleMoves(square, gridToTest);

		for (Point p : allMoves) {
			Piece* pieceOnSquare = gridToTest[square];
			if (pieceOnSquare && pieceOnSquare->type == curType) {
				return true;
			}
		}
	}
	return false;
}

bool GameMode::haveAvailableMoves(Teams team, ChessGrid& gridToTest) {
	//TODO ����� ������� availableMoves ����� ������ ��������, ��� ���� ������ ��� �� � ������� ����� �� ������ �� ��� ������
	// �� ����������� ��� ����� � �� ������� �� �������, ��� ������ �������� ������
	// ����� �� ������ ����� ������ ���, ��� �� ��� ������������ ���, �� ��� ����� �� ������ ����� ������������
	return true;
}

void GameMode::placeTeamPieces(const Teams team, short row, short direction) {

	mainGrid.setPiece({ row, 0 }, new Rook	(team));
	mainGrid.setPiece({ row, 7 }, new Rook	(team));
	mainGrid.setPiece({ row, 1 }, new Knight(team));
	mainGrid.setPiece({ row, 6 }, new Knight(team));
	mainGrid.setPiece({ row, 2 }, new Bishop(team));
	mainGrid.setPiece({ row, 5 }, new Bishop(team));
	mainGrid.setPiece({ row, 3 }, new Queen	(team));
	mainGrid.setPiece({ row, 4 }, new King	(team));

	row += direction;
	for (short col = 0; col < 8; ++col) {
		mainGrid.setPiece({ row, col }, new Pawn(team, direction));
	}

	
}
