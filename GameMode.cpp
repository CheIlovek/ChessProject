#include "GameMode.h"


GameMode::GameMode() : grid(){
}

ChessGrid* GameMode::getGrid() {
	return &grid;
}

void GameMode::resetBoard() {
	grid.clear();
	placeTeamPieces(Teams::WHITE, 7, -1);
	placeTeamPieces(Teams::BLACK, 0, 1);
}

bool GameMode::putPiece(Point gridCoords) {
	// �� ���� ������ �� ���� ������� - ����� ������
	if (!pickedUpPiece) {
		return false;
	}
	// ��� ���������
	if (std::find(pickedUpPieceAvailableMoves.begin(), pickedUpPieceAvailableMoves.end(), gridCoords) == pickedUpPieceAvailableMoves.end()) {
		return false;
	}
	// TODO
	delete grid[gridCoords];
	grid[gridCoords] = pickedUpPiece.release();

	// �������� ��� �� ���, ��� ���
	teamToMove = teamToMove == Teams::WHITE ? Teams::BLACK : Teams::WHITE;
	if (isKingUnderAttack(teamToMove)) {
		if (haveAvailableMoves(teamToMove)) {
			state = GameStates::CHECK;
		} else {
			state = GameStates::CHECKMATE;
		}
	}
}

void GameMode::pickUpPiece(Point gridCoords) {
	// ���� ��� ������ - ����� ������� ����� (� ����� ��� �����??!?!?!??!?!??!)
	if (pickedUpPiece) return;


	Piece* piece = grid.getOrDefault(gridCoords,nullptr);
	if (piece == nullptr) return;

	if (piece->team != teamToMove) return;

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
	
	pickedUpPiece.reset(piece);
	grid[gridCoords] = nullptr;
}

const Piece* GameMode::getPickedUpPiece() {
	return pickedUpPiece.get();
}


std::vector<Point> GameMode::getAllPossibleMovesPawn(Point position) {
	std::vector<Point> possibleMoves;
	Teams team = grid[position]->team;
	short direction = team == Teams::WHITE ? -1 : 1; // ����� ���� "�����", ������ "����"
	Point moveToTest = position.getMovedTo(direction, 0);
	Piece defaultPiece(PiecesTypes::KING, team);	// � ������ ������ �� ����� ��� ��������, 
													// ����� ���� �������� nullptr ������ ���� ������ ���������� � �����.
	
	// ����� �� ����� ������ �����?
	if (grid.getOrDefault(moveToTest, &defaultPiece) == nullptr) {
		possibleMoves.push_back(moveToTest);

		// ���� ������ ��� ����� - �������� ���� �� ��� ������ �����
		if (startRowForBlackPawns == position.row && team == Teams::BLACK ||
			startRowForWhitePawns == position.row && team == Teams::WHITE) {
			moveToTest.moveTo(direction, 0);
			if (grid.getOrDefault(moveToTest, &defaultPiece) == nullptr) {
				possibleMoves.push_back(moveToTest);
			}
		}
	}
	// TODO : �������� �� ������ �� �������


	// ����� �� ������ ������ �������-������?
	moveToTest = position.getMovedTo(direction, 1);
	Piece* piece = grid.getOrDefault(moveToTest, nullptr);
	if (piece && piece->team != team) {
		possibleMoves.push_back(moveToTest);
	}

	// ����� �� ������ ������ �������-�����?
	moveToTest = position.getMovedTo(direction, -1);
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
			Point moveToTest = position.getMovedTo(i, j);
			if (isCorrectMove(moveToTest, team)) {
				possibleMoves.push_back(moveToTest);
			}
			moveToTest = position.getMovedTo(j, i);
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
			Point moveToTest = position.getMovedTo(i, j);
			while (isCorrectMove(moveToTest, team)) {
				possibleMoves.push_back(moveToTest);
				if (!grid[moveToTest]) { // ���� �� ������ ����� ��������� ������ (������� �� ����� ������ ��� ��� �������� isCorrectMove)
					break;
				}
				moveToTest.moveTo(i, j);
			}
		}
	}
	return possibleMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesRook(Point position) {
	std::vector<Point> possibleMoves;
	Teams team = grid[position]->team;

	for (int i = -1; i <= 1; i += 2) {
		Point moveToTest = position.getMovedTo(i, 0);
		while (isCorrectMove(moveToTest, team)) {
			possibleMoves.push_back(moveToTest);
			if (!grid[moveToTest]) { // ���� �� ������ ����� ��������� ������ (������� �� ����� ������ ��� ��� �������� isCorrectMove
				break;
			}
			moveToTest.moveTo(i, 0);
		}
		moveToTest = position.getMovedTo(0, i);
		while (isCorrectMove(moveToTest, team)) {
			possibleMoves.push_back(moveToTest);
			if (!grid[moveToTest]) { // ���� �� ������ ����� ��������� ������ (������� �� ����� ������ ��� ��� �������� isCorrectMove
				break;
			}
			moveToTest.moveTo(0, i);
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
			Point moveToTest = position.getMovedTo(i, j);
			if (isCorrectMove(moveToTest, team)) {
				possibleMoves.push_back(moveToTest);
			}
		}
	}

	return possibleMoves;
}

bool GameMode::isCorrectMove(const Point& position, Teams team) {

	// ������������ defaultValue ��� �� �������, ����� ���������� �������� ����������� ��� ��� �����������
	// (�� ����� ����������, �� ����������) TODO
	// P.S ��� KING - �� �����

	// TODO ��� ���� ������� ����� ����� ������

	Piece defaultPiece(PiecesTypes::KING, team);
	Piece* piece = grid.getOrDefault(position, &defaultPiece);
	return !piece || piece->team != team;
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
	Point pawnLocation = kingLocation.getMovedTo(direction, 1);
	Piece* pawn = grid.getOrDefault(pawnLocation, nullptr);
	if (pawn && pawn->type != PiecesTypes::PAWN && pawn->team != team) {
		return true;
	}
	pawnLocation = kingLocation.getMovedTo(direction, -1);
	pawn = grid.getOrDefault(pawnLocation, nullptr);
	if (pawn && pawn->type != PiecesTypes::PAWN && pawn->team != team) {
		return true;
	}

	return false;
}

bool GameMode::haveAvailableMoves(Teams team) {
	return false;
}

void GameMode::placeTeamPieces(const Teams team, const short row, const short direction) {

	short startPos = row * 8;
	// ���, ��� ��� ������� ���������
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
