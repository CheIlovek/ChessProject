#include <iostream>
#include <SFML/Graphics.hpp>
#include "ChessGrid.h"
#include "ChessboardEntity.h"
#include "Point.h"
int main() {
    sf::RenderWindow window(sf::VideoMode(500,500), "SFML works!");
    sf::View view(sf::FloatRect(0, 0, 2048, 2048));
    window.setView(view);
    window.setVerticalSyncEnabled(true);

    GameMode gameMode;
    ChessboardEntity chessBoardEntity(&gameMode, &window);
    gameMode.resetBoard();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) {
                    std::cout << sf::Mouse::getPosition(window).x << " " << sf::Mouse::getPosition(window).y << '\n';
                    sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    std::cout << worldPos.x << " " << worldPos.y << '\n' << '\n';
                    chessBoardEntity.pickUpPieceAtWorldCoords(worldPos);
                }
                if (event.mouseButton.button == sf::Mouse::Right) {
                    sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    chessBoardEntity.putPieceAtWorldCoords(worldPos);
                }
                break;
            }
   
        }
        window.clear();
        window.draw(chessBoardEntity);
        window.display();
    }

    return 0;
}