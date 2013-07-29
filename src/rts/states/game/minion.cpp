#include "minion.hpp"
#include <SFML/Graphics/RenderTarget.hpp>

namespace
{
    const sf::Time frame_length = sf::seconds(1.0 / 15.0);
}

namespace rts
{
    namespace states
    {
        namespace game
        {
            Minion::Minion(sf::Uint8 playerid, sf::Uint8 hatid, sf::Color colour, sf::Uint16 x, sf::Uint16 y, sf::Texture *texture, sf::Texture *hat_texture, sf::Shader *shader, sf::Uint16 map_size):
                m_x(x),
                m_y(y),
                m_action(Action::STANDING),
                m_hatid(hatid),
                m_colour(colour),
                m_frame(0),
                m_frame_time(frame_length),
                m_direction(2),
                m_playerid(playerid),
                m_alive(true),
                m_selected(false),
                m_texture(texture),
                m_hat_texture(hat_texture),
                m_shader(shader),
                m_path(map_size)
            {
            }

            sf::Uint16 Minion::get_x() const
            {
                return m_x;
            }

            sf::Uint16 Minion::get_y() const
            {
                return m_y;
            }

            void Minion::set_x(sf::Uint16 x)
            {
                m_x = x;
            }

            void Minion::set_y(sf::Uint16 y)
            {
                m_y = y;
            }

            void Minion::set_action(Minion::Action action)
            {
                if(m_action == action) return;
                m_action = action;
                m_frame = 0;
            }

            void Minion::kill()
            {
                m_alive = false;
            }

            bool Minion::alive() const
            {
                return m_alive;
            }

            bool Minion::selected() const
            {
                return m_selected;
            }

            void Minion::select()
            {
                m_selected = true;
            }

            void Minion::deselect()
            {
                m_selected = false;
            }

            sf::Uint8 Minion::get_playerid() const
            {
                return m_playerid;
            }

            void Minion::update(int millis, const std::vector<Tile> &tiles)
            {
                sf::Time dt = sf::milliseconds(millis);

                if(m_action != Action::FIGHTING) 
                    m_action = m_path.move(m_x, m_y, tiles, m_direction) ? Action::WALKING : Action::STANDING;

                if(m_action != Action::STANDING) {
                    m_frame_time += dt;
                    if(m_frame_time > frame_length) {
                        ++m_frame;
                        if(m_action == Action::FIGHTING && m_frame == 8)
                            m_action = Action::STANDING;
                        m_frame = m_frame % 8;
                        m_frame_time -= frame_length;
                    }
                } else {
                    m_path = Path(m_path.get_map_size());
                    m_frame = 0;
                }
            }

            bool Minion::is_moving() const
            {
                return m_action == Action::WALKING;
            }

            void Minion::set_direction(sf::Uint8 direction)
            {
                m_direction = direction;
            }

            sf::FloatRect Minion::get_bounds() const
            {
                return sf::FloatRect(get_x() - 16, get_y() - 39, 32, 48);
            }

            sf::IntRect Minion::get_collision_bounds() const
            {
                return sf::IntRect(get_x() - 16 + 6, get_y() - 32 + 24, 21, 24);
            }

            void Minion::toggle_selection()
            {
                m_selected = !m_selected;
            }

            void Minion::move_to(sf::Uint16 x, sf::Uint16 y, const std::vector<Tile> &tiles, sf::Uint16 map_size)
            {
                m_path = Path(m_x, m_y, x, y, tiles, map_size);
            }

            void Minion::draw(sf::RenderTarget &target, sf::RenderStates states) const
            {
                sf::VertexArray va(sf::Quads, 4);

                va[0].position = sf::Vector2f(m_x - 16, m_y - 39);
                va[1].position = sf::Vector2f(m_x + 15, m_y - 39);
                va[2].position = sf::Vector2f(m_x + 15, m_y + 9);
                va[3].position = sf::Vector2f(m_x - 16, m_y + 9);

                sf::Uint8 action = m_action == Action::FIGHTING ? 4 : 0;
                va[0].texCoords = sf::Vector2f(m_frame * 32, (m_direction + action) * 48);
                va[1].texCoords = sf::Vector2f((m_frame + 1) * 32, (m_direction + action) * 48);
                va[2].texCoords = sf::Vector2f((m_frame + 1) * 32, (m_direction + 1 + action) * 48);
                va[3].texCoords = sf::Vector2f(m_frame * 32, (m_direction + 1 + action) * 48);

                states.texture = m_texture;
                states.shader = m_shader;
                m_shader->setParameter("texture", sf::Shader::CurrentTexture);
                m_shader->setParameter("minion_colour", m_colour);
                target.draw(va, states);

                if(m_hatid == NO_HAT)
                    return;

                va[0].texCoords = sf::Vector2f(m_direction * 32, m_hatid * 48);
                va[1].texCoords = sf::Vector2f((m_direction + 1) * 32, m_hatid * 48);
                va[2].texCoords = sf::Vector2f((m_direction + 1) * 32, (m_hatid + 1) * 48);
                va[3].texCoords = sf::Vector2f(m_direction * 32, (m_hatid + 1) * 48);
                
                if(m_direction % 2 == 1 && m_action == Action::WALKING && (m_frame == 2 || m_frame == 6))
                    for(int i = 0; i < 4; ++i)
                        va[i].texCoords -= sf::Vector2f(0, 1);

                states.texture = m_hat_texture;
                target.draw(va, states);
            }
        }
    }
}
