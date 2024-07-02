#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h" // must be included
#include "Piece.h"
#include "ChessGrid.h"

std::unique_ptr<Piece> Piece::getObjectByTypeAndTeam(Teams team, PiecesTypes type, short moveDirection) {
	switch (type) {
	case PiecesTypes::BISHOP:
		return std::unique_ptr<Piece>(new Bishop(team));
		break;
	case PiecesTypes::KING:
		return std::unique_ptr<Piece>(new King(team));
		break;
	case PiecesTypes::KNIGHT:
		return std::unique_ptr<Piece>(new Knight(team));
		break;
	case PiecesTypes::PAWN:
		return std::unique_ptr<Piece>(new Pawn(team, moveDirection));
		break;
	case PiecesTypes::QUEEN:
		return std::unique_ptr<Piece>(new Queen(team));
		break;
	case PiecesTypes::ROOK:
		return std::unique_ptr<Piece>(new Rook(team));
		break;
	default:
		return nullptr;
	}
}

std::vector<Point> Knight::getAllPossibleMoves(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -2; j <= 2; j += 4) {
			Point moveToTest = position.getMoved(i, j);
			if (gridToTest.getTeamOnSquareOrDefault(moveToTest, Teams::ENEMY_TO_ALL) != team) {
				possibleMoves.push_back(moveToTest);
			}
			moveToTest = position.getMoved(j, i);
			if (gridToTest.getTeamOnSquareOrDefault(moveToTest, Teams::ENEMY_TO_ALL) != team) {
				possibleMoves.push_back(moveToTest);
			}
		}
	}

	return possibleMoves;
}

std::vector<Point> King::getAllPossibleMoves(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) continue;
			Point moveToTest = position.getMoved(i, j);
			if (gridToTest.isValidPoint(moveToTest) && gridToTest.getTeamOnSquareOrDefault(moveToTest, Teams::ENEMY_TO_ALL) != team) {
				possibleMoves.push_back(moveToTest);
			}
		}
	}

	return possibleMoves;
}

std::vector<Point> Pawn::getAllPossibleMoves(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;

	//Можно ли шагнуть вперёд?
	Point moveToTest = position.getMoved(moveDirection, 0);
	if (gridToTest.getTeamOnSquareOrDefault(moveToTest, Teams::ENEMY_TO_ALL) == Teams::ENEMY_TO_ALL) {
		possibleMoves.push_back(moveToTest);
	}


	// Можно ли съесть фигуру спереди-справа?
	moveToTest = position.getMoved(moveDirection, 1);
	if (gridToTest.getTeamOnSquareOrDefault(moveToTest, team) != team) {
		possibleMoves.push_back(moveToTest);
	}

	// Можно ли съесть фигуру спереди-слева?
	moveToTest = position.getMoved(moveDirection, -1);
	if (gridToTest.getTeamOnSquareOrDefault(moveToTest, team) != team) {
		possibleMoves.push_back(moveToTest);
	}

	return possibleMoves;

}

std::vector<Point> Rook::getAllPossibleMoves(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;

	for (int i = -1; i <= 1; i += 2) {
		Point moveToTest = position.getMoved(i, 0);
		while (gridToTest.isValidPoint(moveToTest) && gridToTest.getTeamOnSquareOrDefault(moveToTest, Teams::ENEMY_TO_ALL) != team) {
			possibleMoves.push_back(moveToTest);
			if (gridToTest[moveToTest]) { // Если на клетке стоит вражеская фигура (союзная не может стоять так как сработал isCorrectMove
				break;
			}
			moveToTest.move(i, 0);
		}
		moveToTest = position.getMoved(0, i);
		while (gridToTest.isValidPoint(moveToTest) && gridToTest.getTeamOnSquareOrDefault(moveToTest, Teams::ENEMY_TO_ALL) != team) {
			possibleMoves.push_back(moveToTest);
			if (gridToTest[moveToTest]) { // Если на клетке стоит вражеская фигура (союзная не может стоять так как сработал isCorrectMove
				break;
			}
			moveToTest.move(0, i);
		}
	}

	return possibleMoves;
}

std::vector<Point> Bishop::getAllPossibleMoves(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;


	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			Point moveToTest = position.getMoved(i, j);
			while (gridToTest.isValidPoint(moveToTest) && gridToTest.getTeamOnSquareOrDefault(moveToTest, Teams::ENEMY_TO_ALL) != team) {
				possibleMoves.push_back(moveToTest);
				if (gridToTest[moveToTest]) { // Если на клетке стоит вражеская фигура (союзная не может стоять так как сработал while)
					break;
				}
				moveToTest.move(i, j);
			}
		}
	}

	return possibleMoves;
}

std::vector<Point> Queen::getAllPossibleMoves(Point position, ChessGrid& gridToTest) {
	std::vector<Point> possibleMoves;

	for (int i = -1; i <= 1; i += 2) {
		for (int j = -1; j <= 1; j += 2) {
			Point moveToTest = position.getMoved(i, j);
			while (gridToTest.isValidPoint(moveToTest) && gridToTest.getTeamOnSquareOrDefault(moveToTest, Teams::ENEMY_TO_ALL) != team) {
				possibleMoves.push_back(moveToTest);
				if (gridToTest[moveToTest]) { // Если на клетке стоит вражеская фигура (союзная не может стоять так как сработал while)
					break;
				}
				moveToTest.move(i, j);
			}
		}
	}

	for (int i = -1; i <= 1; i += 2) {
		Point moveToTest = position.getMoved(i, 0);
		while (gridToTest.isValidPoint(moveToTest) && gridToTest.getTeamOnSquareOrDefault(moveToTest, Teams::ENEMY_TO_ALL) != team) {
			possibleMoves.push_back(moveToTest);
			if (gridToTest[moveToTest]) { // Если на клетке стоит вражеская фигура (союзная не может стоять так как сработал isCorrectMove
				break;
			}
			moveToTest.move(i, 0);
		}
		moveToTest = position.getMoved(0, i);
		while (gridToTest.isValidPoint(moveToTest) && gridToTest.getTeamOnSquareOrDefault(moveToTest, Teams::ENEMY_TO_ALL) != team) {
			possibleMoves.push_back(moveToTest);
			if (gridToTest[moveToTest]) { // Если на клетке стоит вражеская фигура (союзная не может стоять так как сработал isCorrectMove
				break;
			}
			moveToTest.move(0, i);
		}
	}

	return possibleMoves;
}

std::ostream& operator<<(std::ostream& os, const Piece& p) {
	std::string teamTitle, typeTitle = "EMPTY";
	switch (p.team) {
	case Teams::BLACK:
		teamTitle = "Black";
		break;
	case Teams::WHITE:
		teamTitle = "White";
		break;
	case Teams::ENEMY_TO_ALL:
		teamTitle = "Enemy to all";
		break;
	}

	return os << "(Team: " << teamTitle << ", Type: " << typeTitle << ")";
}
