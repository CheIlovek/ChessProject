#include <iostream>
#include "GameMode.h"
#include "Piece.h"
#include "spdlog/spdlog.h" 

GameMode::GameMode() : grid(){
}

ChessGrid* GameMode::getGrid() {
	return &grid;
}

void GameMode::squareInteraction(Point gridCoords) {

	// ������� ������ �� �����
	if (gridCoords == pickedUpPiece.piecePreviousPosition) {
		grid[gridCoords] = pickedUpPiece.clear();
		spdlog::info("Figure is placed back");
		return;
	}

	
	if (!pickedUpPiece.piece) { // ���� ������ �� ����� - ���������� ����� � ����� ��������
		pickUpPiece(gridCoords);
	} else {					// ����� - ��������� ������ �� ��� �����.
		makeMove(gridCoords);
	}
}

void GameMode::resetBoard() {
	grid.clear();
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
		delete grid[*enPassantPawn];
		grid[*enPassantPawn] = nullptr;
	}


	// ��� ������ �� 2 �����
	if (pickedUpPiece.piece->type == PiecesTypes::PAWN && std::abs(pickedUpPiece.piecePreviousPosition.row - gridCoords.row) == 2) {
		enPassantPawn.reset(new Point(gridCoords));
	} else {
		enPassantPawn.reset();
	}


	delete grid[gridCoords];
	grid[gridCoords] = pickedUpPiece.clear();
	spdlog::info("Piece is placed");

	// �������� ��� �� ���, ��� ���
	teamToMove = teamToMove == Teams::WHITE ? Teams::BLACK : Teams::WHITE;
	if (isKingUnderAttack(teamToMove)) {
		if (haveAvailableMoves(teamToMove)) {
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
	Piece* piece = grid.getOrDefault(gridCoords,nullptr);
	if (piece == nullptr) {
		spdlog::debug("Tried to pick up empty square");
		return;
	}

	if (piece->team != teamToMove) {
		//TODO
		spdlog::debug("Tried to make move before HIS turn");
		return;
	}

	std::vector<Point> pickedUpPieceAvailableMoves;

	switch (piece->type) {
	case PiecesTypes::BISHOP:
		pickedUpPieceAvailableMoves = getAllPossibleMovesBishop(gridCoords);
		break;
	case PiecesTypes::KING:
		pickedUpPieceAvailableMoves = getAllPossibleMovesKing(gridCoords);
		break;
	case PiecesTypes::KNIGHT:
		pickedUpPieceAvailableMoves = getAllPossibleMovesKnight(gridCoords);
		break;
	case PiecesTypes::PAWN:
		pickedUpPieceAvailableMoves = getAllPossibleMovesPawn(gridCoords);
		break;
	case PiecesTypes::QUEEN:
		pickedUpPieceAvailableMoves = getAllPossibleMovesQueen(gridCoords);
		break;
	case PiecesTypes::ROOK:
		pickedUpPieceAvailableMoves = getAllPossibleMovesRook(gridCoords);
		break;
	}
	pickedUpPiece.init(piece, pickedUpPieceAvailableMoves, gridCoords);
	grid[gridCoords] = nullptr;
	spdlog::info("Piece picked up");
}

const Piece* GameMode::getPickedUpPiece() {
	return pickedUpPiece.piece.get();
}

const std::vector<Point> GameMode::getAllPossibleMovesForPickedUpPiece() const {
	return pickedUpPiece.pieceAvailableMoves;
}


std::vector<Point> GameMode::getAllPossibleMovesPawn(Point position) {
	std::vector<Point> possibleMoves;
	Teams team = grid[position]->team;
	short direction = team == Teams::WHITE ? -1 : 1; // ����� ���� "�����", ������ "����"
	Point moveToTest = position.getMoved(direction, 0);
	Piece defaultPiece(PiecesTypes::KING, team);	// � ������ ������ �� ����� ��� ��������, 
													// ����� ���� �������� nullptr ������ ���� ������ ���������� � �����.
	
	// ����� �� ����� ������ �����?
	if (grid.getOrDefault(moveToTest, &defaultPiece) == nullptr) {
		possibleMoves.push_back(moveToTest);

		// ���� ������ ��� ����� - �������� ���� �� ��� ������ �����
		if (startRowForBlackPawns == position.row && team == Teams::BLACK ||
			startRowForWhitePawns == position.row && team == Teams::WHITE) {
			moveToTest.move(direction, 0);
			if (grid.getOrDefault(moveToTest, &defaultPiece) == nullptr) {
				possibleMoves.push_back(moveToTest);
			}
		}
	}
	// TODO : �������� �� ������ �� �������
	if (enPassantPawn && enPassantPawn->row == position.row && std::abs(enPassantPawn->col - position.col) == 1) {
		possibleMoves.push_back(enPassantPawn->getMoved(direction, 0));
	}


	// ����� �� ������ ������ �������-������?
	moveToTest = position.getMoved(direction, 1);
	Piece* piece = grid.getOrDefault(moveToTest, nullptr);
	if (piece && piece->team != team) {
		possibleMoves.push_back(moveToTest);
	}

	// ����� �� ������ ������ �������-�����?
	moveToTest = position.getMoved(direction, -1);
	piece = grid.getOrDefault(moveToTest, nullptr);
	if (piece && piece->team != team) {
		possibleMoves.push_back(moveToTest);
	}

	return possibleMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesKnight(Point position) {
	std::vector<Point> possibleMoves;
	Teams team = grid[position]->team;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -2; j <= 2; j += 4) {
			Point moveToTest = position.getMoved(i, j);
			if (isCorrectMove(moveToTest, team)) {
				possibleMoves.push_back(moveToTest);
			}
			moveToTest = position.getMoved(j, i);
			if (isCorrectMove(moveToTest, team)) {
				possibleMoves.push_back(moveToTest);
			}
		}
	}
	
	return possibleMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesBishop(Point position) {
	std::vector<Point> possibleMoves;
	Teams team = grid[position]->team;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			Point moveToTest = position.getMoved(i, j);
			while (isCorrectMove(moveToTest, team)) {
				possibleMoves.push_back(moveToTest);
				if (grid[moveToTest]) { // ���� �� ������ ����� ��������� ������ (������� �� ����� ������ ��� ��� �������� isCorrectMove)
					break;
				}
				moveToTest.move(i, j);
			}
		}
	}
	return possibleMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesRook(Point position) {
	std::vector<Point> possibleMoves;
	Teams team = grid[position]->team;

	for (int i = -1; i <= 1; i += 2) {
		Point moveToTest = position.getMoved(i, 0);
		while (isCorrectMove(moveToTest, team)) {
			possibleMoves.push_back(moveToTest);
			if (grid[moveToTest]) { // ���� �� ������ ����� ��������� ������ (������� �� ����� ������ ��� ��� �������� isCorrectMove
				break;
			}
			moveToTest.move(i, 0);
		}
		moveToTest = position.getMoved(0, i);
		while (isCorrectMove(moveToTest, team)) {
			possibleMoves.push_back(moveToTest);
			if (grid[moveToTest]) { // ���� �� ������ ����� ��������� ������ (������� �� ����� ������ ��� ��� �������� isCorrectMove
				break;
			}
			moveToTest.move(0, i);
		}
	}
	
	return possibleMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesQueen(Point position) {
	std::vector<Point> resMoves = getAllPossibleMovesRook(position);
	std::vector<Point> bishopMoves = getAllPossibleMovesBishop(position);
	resMoves.insert(resMoves.end(), bishopMoves.begin(), bishopMoves.end());
	return resMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesKing(Point position) {
	std::vector<Point> possibleMoves;
	Teams team = grid[position]->team;

	// TODO : ������ ������ ���� �� ����� ��� �����
	// �� �������� ��� � ����� ��� ����� ���

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j ++) {
			Point moveToTest = position.getMoved(i, j);
			if (isCorrectMove(moveToTest, team)) {
				possibleMoves.push_back(moveToTest);
			}
		}
	}

	return possibleMoves;
}

bool GameMode::isCorrectMove(const Point position, Teams team) {

	// ������������ defaultValue ��� �� �������, ����� ���������� �������� ����������� ��� ��� �����������
	// (�� ����� ����������, �� ����������) TODO
	// P.S ��� PAWN - �� �����

	// TODO ��� ���� ������� ����� ����� ������
	Piece defaultPiece(PiecesTypes::PAWN, team);
	Piece* piece = grid.getOrDefault(position, &defaultPiece);
	return piece == nullptr || piece->team != team;
}

bool GameMode::isKingUnderAttack(Teams team) {
	// ���� ������
	Point kingLocation = grid.find(PiecesTypes::KING, team);
	if (kingLocation == Point(-1,-1)) {
		//TODO: ERROR (������ �����?!)
	}
	// ��� ��� ������ ������ ����� ��������� �� �����, �� �������� ������ ���
	// ��� �������� ���� ����������� ����� ��������� ����� �� ������� ������ ��� ������ �����
	// � ����������� ���� ����� � ���������� ������� ��� ����
	// �������� ��� ���� ����� �����!!!!!
	// TODO: ����� �������, ����� ����������

	// �������� ����?
	std::vector< Point> points = getAllPossibleMovesKnight(kingLocation);
	for (const Point& point : points) {
		const Piece* piece = grid[point];
		// getAllPossibleMoves �� ��������� ���� �� ������� ������, �������� �� ������� �� �����
		if (piece && piece->type == PiecesTypes::KNIGHT) { 
			return true;
		}
	}
	// �������� ������/������?
	points = getAllPossibleMovesBishop(kingLocation);
	for (const Point& point : points) {
		const Piece* piece = grid[point];
		// getAllPossibleMoves �� ��������� ���� �� ������� ������, �������� �� ������� �� �����
		if (piece && (piece->type == PiecesTypes::BISHOP || piece->type == PiecesTypes::QUEEN)) {
			return true;
		}
	}
	// �������� ������/������?
	points = getAllPossibleMovesRook(kingLocation);
	for (const Point& point : points) {
		const Piece* piece = grid[point];
		// getAllPossibleMoves �� ��������� ���� �� ������� ������, �������� �� ������� �� �����
		if (piece && (piece->type == PiecesTypes::BISHOP || piece->type == PiecesTypes::QUEEN)) {
			return true;
		}
	}
	// �������� ������?
	int direction = team == Teams::WHITE ? -1 : 1;
	Point pawnLocation = kingLocation.getMoved(direction, 1);
	Piece* pawn = grid.getOrDefault(pawnLocation, nullptr);
	if (pawn && pawn->type != PiecesTypes::PAWN && pawn->team != team) {
		return true;
	}
	pawnLocation = kingLocation.getMoved(direction, -1);
	pawn = grid.getOrDefault(pawnLocation, nullptr);
	if (pawn && pawn->type != PiecesTypes::PAWN && pawn->team != team) {
		return true;
	}

	return false;
}

bool GameMode::haveAvailableMoves(Teams team) {
	//TODO ����� ������� availableMoves ����� ������ ��������, ��� ���� ������ ��� �� � ������� ����� �� ������ �� ��� ������
	// �� ����������� ��� ����� � �� ������� �� �������, ��� ������ �������� ������
	// ����� �� ������ ����� ������ ���, ��� �� ��� ������������ ���, �� ��� ����� �� ������ ����� ������������
	return true;
}

void GameMode::placeTeamPieces(const Teams team, const short row, const short direction) {

	short startPos = row * 8;
	grid[startPos + 0] = new Piece(PiecesTypes::ROOK,	team);
	grid[startPos + 7] = new Piece(PiecesTypes::ROOK,	team);
	grid[startPos + 1] = new Piece(PiecesTypes::KNIGHT, team);
	grid[startPos + 6] = new Piece(PiecesTypes::KNIGHT, team);
	grid[startPos + 2] = new Piece(PiecesTypes::BISHOP, team);
	grid[startPos + 5] = new Piece(PiecesTypes::BISHOP, team);
	grid[startPos + 3] = new Piece(PiecesTypes::QUEEN,	team);
	grid[startPos + 4] = new Piece(PiecesTypes::KING,	team);

	startPos += 8 * direction;
	for (int pos = startPos; pos < startPos + 8; ++pos) {
		grid[pos] = new Piece(PiecesTypes::PAWN, team);
	}

	
}
