#ifndef MINION_HPP
#define MINION_HPP

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <SFML/Config.hpp>

namespace rts
{
    namespace states
    {
        namespace game
        {
            class Minion : public sf::Drawable
            {
                public:
                    enum { NO_HAT = 254 };

                    Minion(sf::Uint8 hatid, sf::Color color, sf::Uint16 x, sf::Uint16 y, sf::Texture *texture, sf::Texture *hat_texture, sf::Shader *shader);

                    void update(sf::Time dt);

                    sf::Uint16 get_x();
                    sf::Uint16 get_y();
                    void set_x(sf::Uint16 x);
                    void set_y(sf::Uint16 y);

                    void set_direction(sf::Uint8 direction);
                    void set_moving(bool moving);

                private:
                    void draw(sf::RenderTarget &target, sf::RenderStates states) const;

                private:
                    sf::Uint16 m_x;
                    sf::Uint16 m_y;

                    sf::Uint8 m_hatid;
                    sf::Color m_colour;
                    sf::Uint8 m_frame;
                    sf::Time m_frame_time;
                    sf::Uint8 m_direction;

                    bool m_moving;
                    
                    sf::Texture *m_texture;
                    sf::Texture *m_hat_texture;
                    sf::Shader *m_shader;
            };
        }
    }
}

#endif
