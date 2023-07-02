#pragma once

#include <SFML/Graphics.hpp>

class Air {
  public:
    Air(size_t x, size_t y) {
        m_position.x = x;
        m_position.y = y;
        m_prev_position.x = x;
        m_prev_position.y = y;
        m_velocity.x = 0;
        m_velocity.y = 0;
    }
    sf::Vector2<size_t> m_position;
    sf::Vector2<size_t> m_prev_position;
    sf::Vector2<float> m_velocity;
    float inertia_resistance = 0.0f;
    bool m_freefall = false;
    sf::Color m_color;
};
