#include "grid.hpp"

Grid::Grid(size_t width, size_t height, int scale, sf::RenderTexture& texture) : m_scale(scale), m_width(width / scale), m_height(height / scale), m_texture(texture) {
    objects_va.setPrimitiveType(sf::Quads);
    objects_va.resize(m_width * m_height * 4);
    m_grid.reserve(m_width * m_height);
    for (size_t i = 0; i < m_width * m_height; i++) {
        m_grid.emplace_back(Air{i % m_width, i / m_width});
    }
}

size_t Grid::TransformToGridIndex(size_t x, size_t y) {
    return x + y * m_width;
}

Grid::Cell& Grid::GetCellFromPosition(size_t x, size_t y) {
    return m_grid.at(TransformToGridIndex(x, y));
}

bool Grid::InBoundsPos(size_t x, size_t y) {
    return y >= 0 && y < m_height && x >= 0 && x < m_width;
}

bool Grid::IsAir(size_t x, size_t y) {
    return InBoundsPos(x, y) && std::holds_alternative<Air>(GetCellFromPosition(x, y));
}

bool Grid::IsSand(size_t x, size_t y) {
    return InBoundsPos(x, y) && std::holds_alternative<SandRef>(GetCellFromPosition(x, y));
}

bool Grid::IsWater(size_t x, size_t y) {
    return InBoundsPos(x, y) && std::holds_alternative<WaterRef>(GetCellFromPosition(x, y));
}

bool Grid::IsWall(size_t x, size_t y) {
    return InBoundsPos(x, y) && std::holds_alternative<WallRef>(GetCellFromPosition(x, y));
}

size_t Grid::GetX(Cell& cell) {
    return visit_cell<size_t>(cell, [](auto&& p) { return p.m_position.x; });
}

size_t Grid::GetY(Cell& cell) {
    return visit_cell<size_t>(cell, [](auto&& p) { return p.m_position.y; });
}

void Grid::SwapCells(Cell& c1, Cell& c2) {
    visit_cell<void>(c1, [&](auto& p1) {
        visit_cell<void>(c2, [&](auto& p2) {
            std::swap(p1.m_position, p2.m_position);
            std::swap(p1.m_prev_position, p2.m_prev_position);
            if (std::is_same_v<decltype(p1), decltype(p2)>) {
                std::swap(p1.m_color, p2.m_color);
            }
        });
    });
    std::swap(c1, c2);
}

void Grid::TickSand(float dt) {
    for (auto& particle : m_sand) {
        auto& cell = GetCellFromParticle(particle);

        particle.m_freefall = particle.m_position != particle.m_prev_position;

        if (IsAir(particle.m_position.x, particle.m_position.y + 1)) {
            particle.m_freefall = true;
            particle.m_velocity.y += m_gravity * dt;
            particle.m_velocity.y = std::round(std::min(particle.m_velocity.y, m_terminal_velocity));
        } else {
            particle.m_velocity.y = 0;
        }

        if (particle.m_freefall) {
            for (int i = 1; i <= particle.m_velocity.y; i++) {
                if (!(IsAir(particle.m_position.x, particle.m_position.y + i))) {
                    particle.m_velocity.y = i - 1;
                    break;
                } else if (IsWater(particle.m_position.x, particle.m_position.y + i)) {
                    particle.m_velocity.y = i - 1;
                    visit_cell<void>(GetCellFromPosition(particle.m_position.x, particle.m_position.y + i), [&](auto& p) {

                    });
                    break;
                }
            }
            SwapCells(cell, GetCellFromPosition(particle.m_position.x, particle.m_position.y + particle.m_velocity.y));
        } else {
            auto chance = std::uniform_real_distribution<double>{0, 1}(gen);
            if (chance > 0.5) {
                if (IsAir(particle.m_position.x - 1, particle.m_position.y + 1) || IsWater(particle.m_position.x - 1, particle.m_position.y + 1)) {
                    SwapCells(cell, GetCellFromPosition(particle.m_position.x - 1, particle.m_position.y + 1));
                }
            } else if (IsAir(particle.m_position.x + 1, particle.m_position.y + 1) || IsWater(particle.m_position.x + 1, particle.m_position.y + 1)) {
                SwapCells(cell, GetCellFromPosition(particle.m_position.x + 1, particle.m_position.y + 1));
            }
        }

        particle.m_prev_position = particle.m_position;
    }
}

void Grid::TickWater(float dt) {
    for (auto& particle : m_water) {
        auto& cell = GetCellFromParticle(particle);

        particle.m_freefall = particle.m_position != particle.m_prev_position;

        if (IsAir(particle.m_position.x, particle.m_position.y + 1)) {
            particle.m_freefall = true;
            particle.m_velocity.y += m_gravity * dt;
            particle.m_velocity.y = std::round(std::min(particle.m_velocity.y, m_terminal_velocity));
        } else {
            particle.m_velocity.y = 0;
        }

        if (particle.m_freefall) {
            for (int i = 1; i <= particle.m_velocity.y; i++) {
                if (!IsAir(particle.m_position.x, particle.m_position.y + i)) {
                    particle.m_velocity.y = i - 1;
                    break;
                }
            }
            SwapCells(cell, GetCellFromPosition(particle.m_position.x, particle.m_position.y + particle.m_velocity.y));
        } else {
            int random_dist = std::uniform_int_distribution<int>{0, 10}(gen);
            int direction = std::uniform_int_distribution<int>{0, 1}(gen) ? -1 : 1;
            particle.m_velocity.x = random_dist;
            for (int i = 1; i <= particle.m_velocity.x; i++) {
                if (!IsAir(particle.m_position.x + (i * direction), particle.m_position.y)) {
                    particle.m_velocity.x = i - 1;
                    break;
                }
            }
            SwapCells(cell, GetCellFromPosition(particle.m_position.x + (particle.m_velocity.x * direction), particle.m_position.y));
        }

        particle.m_prev_position = particle.m_position;
    }
}

void Grid::TickWall(float dt) {
    // interaction with different elements
}

void Grid::Tick(float dt) {
    TickSand(dt);
    TickWater(dt);
}

void Grid::DrawSand() {
    MakeVA(m_sand);
}

void Grid::DrawWater() {
    MakeVA(m_water);
}

void Grid::DrawWall() {
    MakeVA(m_wall);
}

void Grid::DrawAll() {
    DrawSand();
    DrawWater();
    DrawWall();
    m_texture.draw(objects_va);
    m_texture.display();
    objects_va.clear();
}

void Grid::AddSand(size_t x, size_t y) {
    size_t new_x = (x / m_scale);
    size_t new_y = (y / m_scale);
    if (IsAir(new_x, new_y)) {
        size_t index = m_sand.size();
        m_sand.emplace_back(Sand{new_x, new_y, m_scale});
        m_grid[TransformToGridIndex(new_x, new_y)] = SandRef{index};
    }
}

void Grid::AddWater(size_t x, size_t y) {
    size_t new_x = (x / m_scale);
    size_t new_y = (y / m_scale);
    if (IsAir(new_x, new_y)) {
        size_t index = m_water.size();
        m_water.emplace_back(Water{new_x, new_y, m_scale});
        m_grid[TransformToGridIndex(new_x, new_y)] = WaterRef{index};
    }
}

void Grid::AddWall(size_t x, size_t y) {
    size_t new_x = (x / m_scale);
    size_t new_y = (y / m_scale);
    if (IsWall(new_x, new_y)) {
        return;
    }
    RemoveCell(GetCellFromPosition(new_x, new_y));
    size_t index = m_wall.size();
    m_wall.emplace_back(Wall{new_x, new_y, m_scale});
    m_grid[TransformToGridIndex(new_x, new_y)] = WallRef{index};
}

void Grid::RemoveCell(Cell& cell) {
    visit_cell_vector(cell, [&](auto& vec) {
        visit_cell<void>(cell, [&](auto& p) {
            p.m_velocity.y = 0;
            p.m_velocity.x = 0;
            p.m_freefall = false;
        });
        SwapCells(cell, GetCellFromParticle(vec.back()));
        vec.pop_back();
    });
    cell = Air{GetX(cell), GetY(cell)};
}

void Grid::RemoveAll() {
    m_sand.clear();
    m_water.clear();
    m_wall.clear();
    for (size_t i = 0; i < m_width * m_height; i++) {
        m_grid[i] = Air(i % m_width, i / m_width);
    }
}

void Grid::DeleteParticleAt(size_t x, size_t y) {
    size_t new_x = (x / m_scale);
    size_t new_y = (y / m_scale);
    if (InBoundsPos(new_x, new_y) && !IsAir(new_x, new_y)) {
        RemoveCell(GetCellFromPosition(new_x, new_y));
    }
}
