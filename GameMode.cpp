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

	// Вернуть фигуру на место
	if (gridCoords == pickedUpPiece.piecePreviousPosition) {
		mainGrid.setPiece(
			pickedUpPiece.piecePreviousPosition, 
			pickedUpPiece.piece.release());
		spdlog::info("Figure is placed back");
		pickedUpPiece.clear();
		return;
	}

	if (!pickedUpPiece.piece) { // Если фигура не взята - попытаться взять с этого квадрата
		pickUpPiece(gridCoords);
	} else {					// Иначе - поставить фигуру на это место.
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

	// Ход нелегален
	std::vector<Point>& availableMoves = pickedUpPiece.pieceAvailableMoves;
	if (std::find(availableMoves.begin(), availableMoves.end(), gridCoords) == availableMoves.end()) {
		spdlog::debug("Tried to make illegal move");
		return false;
	}

	// Если это взятие на проходе - то нужно удалять фигуру enPassantPawn
	short direction = pickedUpPiece.piece->team == Teams::WHITE ? -1 : 1;
	if (enPassantPawn && pickedUpPiece.piece->type == PiecesTypes::PAWN && gridCoords.col == enPassantPawn->col && enPassantPawn->row + direction == gridCoords.row) {
		mainGrid.removePiece(*enPassantPawn);
	}


	// Ход пешкой на 2 вперёд
	if (pickedUpPiece.piece->type == PiecesTypes::PAWN && std::abs(pickedUpPiece.piecePreviousPosition.row - gridCoords.row) == 2) {
		enPassantPawn.reset(new Point(gridCoords));
	} else {
		enPassantPawn.reset();
	}

	mainGrid.setPiece(gridCoords, pickedUpPiece.clear());
	spdlog::info("Piece is placed");

	// Проверка мат ли это, или шах
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

	std::vector<Point> pickedUpPieceAvailableMoves;

	switch (piece->type) {
	case PiecesTypes::BISHOP:
		pickedUpPieceAvailableMoves = getAllPossibleMovesBishop	(gridCoords, mainGrid);
		break;
	case PiecesTypes::KING:
		pickedUpPieceAvailableMoves = getAllPossibleMovesKing	(gridCoords, mainGrid);
		break;
	case PiecesTypes::KNIGHT:
		pickedUpPieceAvailableMoves = getAllPossibleMovesKnight	(gridCoords, mainGrid);
		break;
	case PiecesTypes::PAWN:
		pickedUpPieceAvailableMoves = getAllPossibleMovesPawn	(gridCoords, mainGrid);
		break;
	case PiecesTypes::QUEEN:
		pickedUpPieceAvailableMoves = getAllPossibleMovesQueen	(gridCoords, mainGrid);
		break;
	case PiecesTypes::ROOK:
		pickedUpPieceAvailableMoves = getAllPossibleMovesRook	(gridCoords, mainGrid);
		break;
	}
	pickedUpPiece.init(piece, pickedUpPieceAvailableMoves, gridCoords);
	mainGrid.removePiece(gridCoords);
	spdlog::info("Piece picked up");
}

const Piece* GameMode::getPickedUpPiece() {
	return pickedUpPiece.piece.get();
}

const std::vector<Point> GameMode::getAllPossibleMovesForPickedUpPiece() const {
	return pickedUpPiece.pieceAvailableMoves;
}


std::vector<Point> GameMode::getAllPossibleMovesPawn(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;
	Teams team = gridToTest[position]->team;

	bool isKingCouldBeAttacked = couldThreatTheKingByMove(team, position, gridToTest);

	short direction = team == Teams::WHITE ? -1 : 1; // Белые идут "вверх", чёрные "вниз"
	Point moveToTest = position.getMoved(direction, 0);
	Piece defaultPiece(PiecesTypes::KING, team);	// В данном случае не важно его значение, 
													// нужен чтоб получить nullptr только если клетка существует и пуста.
	
	// Может ли пешка пройти вперёд?
	if (gridToTest.getOrDefault(moveToTest, &defaultPiece) == nullptr) {
		if (isMoveSafe(team, position, moveToTest, gridToTest)) {
			possibleMoves.push_back(moveToTest);
		}

		// Если первый ход пешки - проверка хода на две клетки вперёд
		if (startRowForBlackPawns == position.row && team == Teams::BLACK ||
			startRowForWhitePawns == position.row && team == Teams::WHITE) {
			moveToTest.move(direction, 0);
			if (gridToTest.getOrDefault(moveToTest, &defaultPiece) == nullptr && isMoveSafe(team, position, moveToTest, gridToTest)) {
				possibleMoves.push_back(moveToTest);
			}
		}
	}
	// TODO : Проверка на ВЗЯТИЕ НА ПРОХОДЕ
	if (enPassantPawn) {
		moveToTest = enPassantPawn->getMoved(direction, 0);
		if (enPassantPawn->row == position.row && std::abs(enPassantPawn->col - position.col) == 1 && isMoveSafe(team, position, moveToTest, gridToTest)) {
			possibleMoves.push_back(moveToTest);
		}
	}
	


	// Можно ли съесть фигуру спереди-справа?
	moveToTest = position.getMoved(direction, 1);
	Piece* piece = gridToTest.getOrDefault(moveToTest, nullptr);
	if (piece && piece->team != team && isMoveSafe(team, position, moveToTest, gridToTest)) {
		possibleMoves.push_back(moveToTest);
	}

	// Можно ли съесть фигуру спереди-слева?
	moveToTest = position.getMoved(direction, -1);
	piece = gridToTest.getOrDefault(moveToTest, nullptr);
	if (piece && piece->team != team && isMoveSafe(team, position, moveToTest, gridToTest)) {
		possibleMoves.push_back(moveToTest);
	}

	return possibleMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesKnight(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;
	Teams team = gridToTest[position]->team;

	bool isKingCouldBeAttacked = couldThreatTheKingByMove(team, position, gridToTest);

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -2; j <= 2; j += 4) {
			Point moveToTest = position.getMoved(i, j);
			if (isCorrectMove(position, moveToTest, team, gridToTest, isKingCouldBeAttacked)) {
				possibleMoves.push_back(moveToTest);
			}
			moveToTest = position.getMoved(j, i);
			if (isCorrectMove(position, moveToTest, team, gridToTest, isKingCouldBeAttacked)) {
				possibleMoves.push_back(moveToTest);
			}
		}
	}
	
	return possibleMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesBishop(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;
	Teams team = gridToTest[position]->team;

	bool isKingCouldBeAttacked = couldThreatTheKingByMove(team, position, gridToTest);

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			Point moveToTest = position.getMoved(i, j);
			while (isCorrectMove(position, moveToTest, team, gridToTest, isKingCouldBeAttacked)) {
				possibleMoves.push_back(moveToTest);
				if (gridToTest[moveToTest]) { // Если на клетке стоит вражеская фигура (союзная не может стоять так как сработал isCorrectMove)
					break;
				}
				moveToTest.move(i, j);
			}
		}
	}
	return possibleMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesRook(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;
	Teams team = gridToTest[position]->team;

	bool isKingCouldBeAttacked = couldThreatTheKingByMove(team, position, gridToTest);

	for (int i = -1; i <= 1; i += 2) {
		Point moveToTest = position.getMoved(i, 0);
		while (isCorrectMove(position, moveToTest, team, gridToTest, isKingCouldBeAttacked)) {
			possibleMoves.push_back(moveToTest);
			if (gridToTest[moveToTest]) { // Если на клетке стоит вражеская фигура (союзная не может стоять так как сработал isCorrectMove
				break;
			}
			moveToTest.move(i, 0);
		}
		moveToTest = position.getMoved(0, i);
		while (isCorrectMove(position, moveToTest, team, gridToTest, isKingCouldBeAttacked)) {
			possibleMoves.push_back(moveToTest);
			if (gridToTest[moveToTest]) { // Если на клетке стоит вражеская фигура (союзная не может стоять так как сработал isCorrectMove
				break;
			}
			moveToTest.move(0, i);
		}
	}
	
	return possibleMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesQueen(Point position, ChessGrid& gridToTest) {
	std::vector<Point> resMoves =		getAllPossibleMovesRook		(position, gridToTest);
	std::vector<Point> bishopMoves =	getAllPossibleMovesBishop	(position, gridToTest);
	resMoves.insert(resMoves.end(), bishopMoves.begin(), bishopMoves.end());
	return resMoves;
}

std::vector<Point> GameMode::getAllPossibleMovesKing(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;
	Teams team = gridToTest[position]->team;


	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j ++) {
			Point moveToTest = position.getMoved(i, j);
			if (isCorrectMove(position, moveToTest, team, gridToTest)) {
				possibleMoves.push_back(moveToTest);
			}
		}
	}

	return possibleMoves;
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
	// Устанвливаем defaultValue той же команды, чтобы дальнейшая проверка провалилась при его возвращении
	// (Не очень интуитивно, но эффективно) TODO


	Piece defaultPiece(PiecesTypes::NONE, team);
	Piece* piece = gridToTest.getOrDefault(nextPos, &defaultPiece);
	if (!(!piece || piece->team != team)) {
		return false;
	}

	return !checkKingSafety || isMoveSafe(team, curPos, nextPos, gridToTest);
}

bool GameMode::isKingUnderAttack(Teams team, ChessGrid& gridToTest) {
	// Ищем короля
	Point kingLocation = gridToTest.find(PiecesTypes::KING, team);
	if (kingLocation == Point(-1,-1)) {
		spdlog::error("King is missing!");
		//TODO: ERROR (Король исчез?!)
	}

	

	// Атакован королём?
	for (int i = -1; i <= 1; i ++) {
		for (int j = -1; j <= 1; j ++) {
			Point moveToTest = kingLocation.getMoved(i, j);
			Piece* possibleKing = gridToTest.getOrDefault(moveToTest, nullptr);
			if (possibleKing && possibleKing->type == PiecesTypes::KING && possibleKing->team != team) {
				return true;
			}
		}
	}

	// Атакован пешкой?
	int direction = team == Teams::WHITE ? -1 : 1; // ИНВЕРТИРОВАНО
	Point pawnLocation = kingLocation.getMoved(direction, 1);
	Piece* pawn = gridToTest.getOrDefault(pawnLocation, nullptr);
	if (pawn && pawn->type == PiecesTypes::PAWN && pawn->team != team) {
		return true;
	}
	pawnLocation = kingLocation.getMoved(direction, -1);
	pawn = gridToTest.getOrDefault(pawnLocation, nullptr);
	if (pawn && pawn->type == PiecesTypes::PAWN && pawn->team != team) {
		return true;
	}

	// Атакован конём?
	for (int i = -1; i <= 1; i += 2) {
		for (int j = -2; j <= 2; j += 4) {
			Point moveToTest = kingLocation.getMoved(i, j);
			Piece* possibleKnight = gridToTest.getOrDefault(moveToTest, nullptr);
			if (possibleKnight && possibleKnight->type == PiecesTypes::KNIGHT && possibleKnight->team != team) {
				return true;
			}
			moveToTest = kingLocation.getMoved(j, i);
			possibleKnight = gridToTest.getOrDefault(moveToTest, nullptr);
			if (possibleKnight && possibleKnight->type == PiecesTypes::KNIGHT && possibleKnight->team != team) {
				return true;
			}
		}
	}

	// Атакован ладьей/ферзем?
	for (int i = -1; i <= 1; i+=2) {
			Point moveToTest = kingLocation.getMoved(i, 0);
			while (isCorrectMove(moveToTest, moveToTest, team, gridToTest, false)) {
				Piece* possibleAttacker = gridToTest[moveToTest];
				if (possibleAttacker) { // не может быть союзная фигура - isCorrectMove не сработал бы
					if (possibleAttacker->type == PiecesTypes::ROOK ||
						possibleAttacker->type == PiecesTypes::QUEEN) {
						return true;
					} else {
						break;
					}
				}
				moveToTest.move(i, 0);
			}

			moveToTest = kingLocation.getMoved(0, i);
			while (isCorrectMove(moveToTest, moveToTest, team, gridToTest, false)) {
				Piece* possibleAttacker = gridToTest[moveToTest];
				if (possibleAttacker) { // не может быть союзная фигура - isCorrectMove не сработал бы
					if (possibleAttacker->type == PiecesTypes::ROOK ||
						possibleAttacker->type == PiecesTypes::QUEEN) {
						return true;
					} else {
						break;
					}
				}
				moveToTest.move(0, i);
			}
	}

	// Атакован слоном/ферзем?
	for (int i = -1; i <= 1; i+=2) {
		for (int j = -1; j <= 1; j+=2) {
			Point moveToTest = kingLocation.getMoved(i, j);
			while (isCorrectMove(moveToTest, moveToTest, team, gridToTest, false)) {
				Piece* possibleAttacker = gridToTest[moveToTest];
				if (possibleAttacker) { // не может быть союзная фигура - isCorrectMove не сработал бы
					if (possibleAttacker->type == PiecesTypes::BISHOP ||
						possibleAttacker->type == PiecesTypes::QUEEN) {
						return true;
					} else {
						break;
					}
				}
				moveToTest.move(i, j);
			}
		}
	}

	return false;
}

bool GameMode::haveAvailableMoves(Teams team, ChessGrid& gridToTest) {
	//TODO когда получаю availableMoves нужно делать проверку, что если фигуры нет на её прошлом месте то Король не под атакой
	// НО исколючение это пешки с их взятием на проходе, там фигура меняется дважды
	// БЛЛЛЛ но фигура может отойти так, что всё ещё загараживает шах, но уже стоит на другом месте ептааааааааа
	return true;
}

void GameMode::placeTeamPieces(const Teams team, const short row, const short direction) {

	short startPos = row * 8;
	mainGrid.setPiece(startPos + 0, PiecesTypes::ROOK,		team);
	mainGrid.setPiece(startPos + 7, PiecesTypes::ROOK,		team);
	mainGrid.setPiece(startPos + 1, PiecesTypes::KNIGHT,	team);
	mainGrid.setPiece(startPos + 6, PiecesTypes::KNIGHT,	team);
	mainGrid.setPiece(startPos + 2, PiecesTypes::BISHOP,	team);
	mainGrid.setPiece(startPos + 5, PiecesTypes::BISHOP,	team);
	mainGrid.setPiece(startPos + 3, PiecesTypes::QUEEN,		team);
	mainGrid.setPiece(startPos + 4, PiecesTypes::KING,		team);

	startPos += 8 * direction;
	for (int pos = startPos; pos < startPos + 8; ++pos) {
		mainGrid.setPiece(pos,		PiecesTypes::PAWN,		team);
	}

	
}
