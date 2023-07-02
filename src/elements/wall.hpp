#pragma once

#include <SFML/Graphics.hpp>

class Wall {
  public:
    Wall(size_t x, size_t y, int scale) {
        m_position.x = x;
        m_position.y = y;
        m_prev_position.x = x;
        m_prev_position.y = y;
        m_velocity.x = 0;
        m_velocity.y = 0;
        int c_value = rand() % 50 + 74;
        m_color = sf::Color(c_value, c_value, c_value);
        m_rect.setSize(sf::Vector2f(scale, scale));
    }

    sf::Vector2<size_t> m_position;
    sf::Vector2<size_t> m_prev_position;
    sf::Vector2<float> m_velocity;
    bool m_freefall = false;
    sf::RectangleShape m_rect;
    sf::Color m_color;
};
