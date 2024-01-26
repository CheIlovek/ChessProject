#include <iostream>
#include <SFML/Graphics.hpp>
#include "spdlog/async.h" 
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h" // must be included


#include "ChessGrid.h"
#include "ChessboardEntity.h"
#include "Point.h"








int main() {
    Point c(-1,-1);
    //auto console = spdlog::stdout_logger_mt("console");
    //console->info("custom class with operator<<: {}..", c);
    spdlog::info(c);
    spdlog::info("Welcome!");
    sf::RenderWindow window(sf::VideoMode(500,500), "SFML works!");
    sf::View view(sf::FloatRect(0, 0, 2048, 2048));
    window.setView(view);
    window.setVerticalSyncEnabled(true);
    //// Настройка логгера
    //auto async_logger = 
    //    std::make_shared<
    //        spdlog::async_factory_impl<spdlog::async_overflow_policy::block>>()
    //    ;
    //async_logger->set_level(spdlog::level::level_enum::trace);
    //spdlog::set_default_logger(async_logger);
    spdlog::set_level(spdlog::level::level_enum::trace);


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
                    sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    chessBoardEntity.interactAtWorldCoords(worldPos);
                }
                break;
            }
   
        }
        window.clear();
        window.draw(chessBoardEntity);
        window.display();
    }

    spdlog::shutdown();

    return 0;
}