#include "ChessboardEntity.h"
#include "Piece.h"

ChessboardEntity::ChessboardEntity(GameMode* grid, sf::RenderWindow* window)
    : gameMode(grid), window(window) {
    
    loadSprites();
}

void ChessboardEntity::loadSprites() {
    chessboradTexture.loadFromFile("content/2x2chessTile.png");
    chessboradTexture.setRepeated(true);
    chessboradSprite.setTexture(chessboradTexture);
    chessboradSprite.setTextureRect({ 0, 0, 2048, 2048 });
    piecesTexture.loadFromFile("content/ChessPieces3.png");
    int pieceResolution = 256;
    int pieceHight = 400;
    int pieceWidth = 300;

    blackPiecesSprites[PiecesTypes::BISHOP] =  sf::Sprite(piecesTexture, sf::IntRect(0,                    0,               pieceResolution, pieceResolution));
    blackPiecesSprites[PiecesTypes::KING] =    sf::Sprite(piecesTexture, sf::IntRect(pieceResolution,      0,               pieceResolution, pieceResolution));
    blackPiecesSprites[PiecesTypes::KNIGHT] =  sf::Sprite(piecesTexture, sf::IntRect(pieceResolution * 2,  0,               pieceResolution, pieceResolution));
    blackPiecesSprites[PiecesTypes::PAWN] =    sf::Sprite(piecesTexture, sf::IntRect(pieceResolution * 3,  0,               pieceResolution, pieceResolution));
    blackPiecesSprites[PiecesTypes::QUEEN] =   sf::Sprite(piecesTexture, sf::IntRect(pieceResolution * 4,  0,               pieceResolution, pieceResolution));
    blackPiecesSprites[PiecesTypes::ROOK] =    sf::Sprite(piecesTexture, sf::IntRect(pieceResolution * 5,  0,               pieceResolution, pieceResolution));

    whitePiecesSprites[PiecesTypes::BISHOP] =  sf::Sprite(piecesTexture, sf::IntRect(0,                    pieceResolution, pieceResolution, pieceResolution));
    whitePiecesSprites[PiecesTypes::KING] =    sf::Sprite(piecesTexture, sf::IntRect(pieceResolution,      pieceResolution, pieceResolution, pieceResolution));
    whitePiecesSprites[PiecesTypes::KNIGHT] =  sf::Sprite(piecesTexture, sf::IntRect(pieceResolution * 2,  pieceResolution, pieceResolution, pieceResolution));
    whitePiecesSprites[PiecesTypes::PAWN] =    sf::Sprite(piecesTexture, sf::IntRect(pieceResolution * 3,  pieceResolution, pieceResolution, pieceResolution));
    whitePiecesSprites[PiecesTypes::QUEEN] =   sf::Sprite(piecesTexture, sf::IntRect(pieceResolution * 4,  pieceResolution, pieceResolution, pieceResolution));
    whitePiecesSprites[PiecesTypes::ROOK] =    sf::Sprite(piecesTexture, sf::IntRect(pieceResolution * 5,  pieceResolution, pieceResolution, pieceResolution));

}

void ChessboardEntity::draw(sf::RenderTarget& target, sf::RenderStates states) const {

    states.transform *= getTransform();
    ChessGrid* chessGrid = gameMode->getGrid();
    target.draw(chessboradSprite);
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece* piece = (*chessGrid)[row * 8 + col];
            if (piece == nullptr) continue;
            sf::Sprite sprite;
            if (piece->team == Teams::WHITE) {
                sprite = whitePiecesSprites.at(piece->type);
            } else {
                sprite = blackPiecesSprites.at(piece->type);
            }
            sprite.setPosition(chessSquareResolution * col, chessSquareResolution * row);
            target.draw(sprite, states);
            
        }
    }

    // Отрисовка возможных ходов
    sf::CircleShape circle(50.f);
    circle.setOrigin(50.f, 50.f);
    circle.setFillColor(sf::Color(100, 250, 50, 100));
    auto movesPoints = gameMode->getAllPossibleMovesForPickedUpPiece();
    for (const Point& point : movesPoints) {
        circle.setPosition({ (float)(point.col + 0.5) * chessSquareResolution, (float)(point.row + 0.5) * chessSquareResolution });
        target.draw(circle);
    }


    // Отрисовка поднятой фигуры
    const Piece* pickedUpPiece = gameMode->getPickedUpPiece();
    if (!pickedUpPiece) {
        return;
    }
    sf::Sprite sprite;
    if (pickedUpPiece->team == Teams::WHITE) {
        sprite = whitePiecesSprites.at(pickedUpPiece->type);
    } else {
        sprite = blackPiecesSprites.at(pickedUpPiece->type);
    }
    sf::Vector2f worldMousePos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));

    sprite.setPosition(worldMousePos.x - chessSquareResolution / 2, worldMousePos.y - chessSquareResolution / 2);
    target.draw(sprite, states);

    
}

void ChessboardEntity::interactAtWorldCoords(sf::Vector2f coords) {
    Point pieceCoords(coords.y / chessSquareResolution, coords.x / chessSquareResolution);
    gameMode->squareInteraction(pieceCoords);
}
