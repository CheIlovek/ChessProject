#pragma once
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "GameMode.h"



class ChessboardEntity : public sf::Drawable, public sf::Transformable  {
public:
	ChessboardEntity(GameMode* grid, sf::RenderWindow* window);

	void interactAtWorldCoords(sf::Vector2f coords);
private:
	void loadSprites();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	
	sf::RenderWindow* window;
	
	std::unordered_map<PiecesTypes, sf::Sprite> whitePiecesSprites;
	std::unordered_map<PiecesTypes, sf::Sprite> blackPiecesSprites;
	sf::Sprite chessboradSprite;
	sf::Texture chessboradTexture;
	sf::Texture piecesTexture;
	std::vector<sf::Sprite> piecesSprites;
	GameMode* gameMode;
	int chessSquareResolution = 256;
	
};

