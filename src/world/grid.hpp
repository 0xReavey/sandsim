#pragma once
#include <SFML/Graphics.hpp>
#include <future>
#include <iostream>
#include <random>
#include <variant>
#include <vector>

#include "air.hpp"
#include "sand.hpp"
#include "wall.hpp"
#include "water.hpp"

static std::mt19937 gen(std::random_device{}());

class Grid {
  public:
    std::vector<Sand> m_sand;
    struct SandRef {
        size_t index;
    };

    std::vector<Water> m_water;
    struct WaterRef {
        size_t index;
    };

    std::vector<Wall> m_wall;
    struct WallRef {
        size_t index;
    };

    using Cell = std::variant<Air, SandRef, WaterRef, WallRef>;
    std::vector<Cell> m_grid;

    int m_scale;
    size_t m_width;
    size_t m_height;
    float m_gravity = 98.1;
    float m_terminal_velocity = 20;

    sf::RenderTexture& m_texture;
    sf::VertexArray objects_va;
    std::vector<Cell> m_to_draw;

    Grid(size_t width, size_t height, int scale, sf::RenderTexture& texture);
    size_t TransformToGridIndex(size_t x, size_t y);
    Cell& GetCellFromPosition(size_t x, size_t y);
    bool InBoundsPos(size_t x, size_t y);
    bool IsAir(size_t x, size_t y);
    bool IsSand(size_t x, size_t y);
    bool IsWater(size_t x, size_t y);
    bool IsWall(size_t x, size_t y);
    void TickSand(float dt);
    void TickWater(float dt);
    void TickWall(float dt);
    void Tick(float dt);
    void DrawSand();
    void DrawWater();
    void DrawWall();
    void DrawAll();
    void AddSand(size_t x, size_t y);
    void AddWater(size_t x, size_t y);
    void AddWall(size_t x, size_t y);
    void DeleteParticleAt(size_t x, size_t y);
    void RemoveCell(Cell& cell);
    void RemoveAll();
    void SwapCells(Cell& c1, Cell& c2);
    size_t GetX(Cell& cell);
    size_t GetY(Cell& cell);

    template <typename Particle>
    Cell& GetCellFromParticle(Particle& p) {
        return GetCellFromPosition(p.m_position.x, p.m_position.y);
    }

    template <typename Particle>
    void MakeVA(Particle& particles) {
        auto futures = std::vector<std::future<void>>(std::thread::hardware_concurrency());

        sf::VertexArray temp_va(sf::Quads);
        size_t particle_size = particles.size();

        temp_va.resize(particle_size * 4);

        const auto make_pixels = [&](size_t start, size_t end) {
            for (int i = start; i < end; ++i) {
                const auto& particle = particles[i];
                const int idx = i << 2;
                temp_va[idx + 0].position = sf::Vector2f(particle.m_position.x * m_scale, particle.m_position.y * m_scale) + sf::Vector2f{-m_scale / 2.f, -m_scale / 2.f};
                temp_va[idx + 1].position = sf::Vector2f(particle.m_position.x * m_scale, particle.m_position.y * m_scale) + sf::Vector2f{m_scale / 2.f, -m_scale / 2.f};
                temp_va[idx + 2].position = sf::Vector2f(particle.m_position.x * m_scale, particle.m_position.y * m_scale) + sf::Vector2f{m_scale / 2.f, m_scale / 2.f};
                temp_va[idx + 3].position = sf::Vector2f(particle.m_position.x * m_scale, particle.m_position.y * m_scale) + sf::Vector2f{-m_scale / 2.f, m_scale / 2.f};

                temp_va[idx + 0].color = particle.m_color;
                temp_va[idx + 1].color = particle.m_color;
                temp_va[idx + 2].color = particle.m_color;
                temp_va[idx + 3].color = particle.m_color;
            }
        };

        int batch_size = particle_size / futures.size();
        for (size_t i = 0; i < futures.size(); i++) {
            const size_t start = i * batch_size;
            const size_t end = start + batch_size;
            futures[i] = std::async(std::launch::async, make_pixels, start, end);
        }

        for (auto& future : futures) {
            future.wait();
        }

        if (batch_size * futures.size() < particle_size) {
            const size_t start = batch_size * futures.size();
            make_pixels(start, particle_size);
        }

        for (size_t i = 0; i < temp_va.getVertexCount(); i++) {
            objects_va.append(temp_va[i]);
        }
    }

    template <typename Return, typename Func>
    Return visit_cell(Cell& cell, Func&& func) {
        return std::visit(
            [&](auto& ref) {
                using T = std::decay_t<decltype(ref)>;
                if constexpr (std::is_same_v<T, Air>) {
                    return func(ref);
                } else if constexpr (std::is_same_v<T, SandRef>) {
                    return func(m_sand[ref.index]);
                } else if constexpr (std::is_same_v<T, WaterRef>) {
                    return func(m_water[ref.index]);
                } else if constexpr (std::is_same_v<T, WallRef>) {
                    return func(m_wall[ref.index]);
                }
            },
            cell);
    }

    template <typename Func>
    void visit_cell_vector(Cell& cell, Func&& func) {
        std::visit(
            [&](auto& ref) {
                using T = std::decay_t<decltype(ref)>;
                if constexpr (std::is_same_v<T, Air>) {
                    return;
                } else if constexpr (std::is_same_v<T, SandRef>) {
                    func(m_sand);
                } else if constexpr (std::is_same_v<T, WaterRef>) {
                    func(m_water);
                } else if constexpr (std::is_same_v<T, WallRef>) {
                    func(m_wall);
                }
            },
            cell);
    }
};
