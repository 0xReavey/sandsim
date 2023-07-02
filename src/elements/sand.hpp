#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class Sand {
  public:
    Sand(size_t x, size_t y, int scale) {
        m_position.x = x;
        m_position.y = y;
        m_prev_position.x = x;
        m_prev_position.y = y;
        m_velocity.x = 0;
        m_velocity.y = 0;
        m_color = sf::Color(255, rand() % 8 + 242, rand() % 145);
        m_rect.setSize(sf::Vector2f(scale, scale));
    }

    sf::Vector2<size_t> m_position;
    sf::Vector2<size_t> m_prev_position;
    sf::Vector2<float> m_velocity;
    float m_inertia_resistance = 0.0;
    bool m_freefall = false;
    sf::RectangleShape m_rect;
    sf::Color m_color;
};
