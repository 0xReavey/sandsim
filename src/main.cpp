#include <SFML/Graphics.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "grid.hpp"

using namespace std::chrono;

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "sandsim", sf::Style::Close);

    window.setFramerateLimit(60);

    sf::RenderTexture texture;
    texture.create(1000, 1000);

    sf::Sprite sprite;
    sprite.setTexture(texture.getTexture());

    sf::Clock clock;

    sf::Font font;

    if (!font.loadFromFile("../font/font.ttf")) {
        std::cerr << "Font not found\n";
    }

    sf::Text text;
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setOutlineThickness(2);
    text.setOutlineColor(sf::Color::Black);

    auto lastSecond = steady_clock::now();
    std::chrono::duration<double> tickTime = 0s;
    std::chrono::duration<double> drawTime = 0s;

    bool pause = false;

    Grid grid(1000, 1000, 4, texture);

    while (window.isOpen()) {
        sf::Time delta_time = clock.restart();
        float dt = delta_time.asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::P) {
                    if (pause == false) {
                        pause = true;
                    } else {
                        pause = false;
                    }
                } else if (event.key.code == sf::Keyboard::C) {
                    grid.RemoveAll();
                } else if (event.key.code == sf::Keyboard::Q) {
                    grid.RemoveAll();
                    window.close();
                }
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            auto mouse_pos = sf::Mouse::getPosition(window);
            for (int y = mouse_pos.y - 25; y < mouse_pos.y + 25; y += 4) {
                for (int x = mouse_pos.x - 25; x < mouse_pos.x + 25; x += 4) {
                    grid.AddSand(x, y);
                }
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            auto mouse_pos = sf::Mouse::getPosition(window);
            for (int y = mouse_pos.y - 25; y < mouse_pos.y + 25; y += 4) {
                for (int x = mouse_pos.x - 25; x < mouse_pos.x + 25; x += 4) {
                    grid.AddWater(x, y);
                }
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            auto mouse_pos = sf::Mouse::getPosition(window);
            for (int y = mouse_pos.y - 10; y < mouse_pos.y + 10; y += 4) {
                for (int x = mouse_pos.x - 10; x < mouse_pos.x + 10; x += 4) {
                    grid.AddWall(x, y);
                }
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            auto mouse_pos = sf::Mouse::getPosition(window);
            for (int y = mouse_pos.y - 50; y < mouse_pos.y + 50; y++) {
                for (int x = mouse_pos.x - 50; x < mouse_pos.x + 50; x++) {
                    grid.DeleteParticleAt(x, y);
                }
            }
        }

        window.clear();
        texture.clear({0, 0, 0});

        if (pause == false) {
            auto tick_start = steady_clock::now();
            grid.Tick(dt);
            tickTime += steady_clock::now() - tick_start;
        }

        auto draw_start = steady_clock::now();
        grid.DrawAll();
        drawTime += steady_clock::now() - draw_start;

        window.draw(sprite);
        window.draw(text);
        window.display();

        auto text_builder = std::ostringstream();
        text_builder << std::setw(4) << static_cast<int>(1 / dt) << " fps\n";
        text.setString(text_builder.str());

        if (steady_clock::now() - lastSecond >= 1s) {
            std::cout << "----------------------------------\n";
            std::cout << "Particles = " << grid.m_sand.size() << '\n';
            std::cout << "Fps = " << static_cast<int>(1 / dt) << '\n';
            std::cout << "Tick time = " << duration_cast<milliseconds>(tickTime) << '\n';
            std::cout << "Draw time = " << duration_cast<milliseconds>(drawTime) << '\n';
            lastSecond = steady_clock::now();
            tickTime = 0s;
            drawTime = 0s;
        }
    }

    return 0;
}
