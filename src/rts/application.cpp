#include "application.hpp"

#include <SFML/Window/Event.hpp>

namespace rts
{
    namespace
    {
        const sf::Time TIME_PER_FRAME = sf::seconds(1.0 / 30.0);
    }

    Application::Application():
        m_render_window(
            sf::VideoMode(800, 600), "rts"      
        ),
        m_state_stack(
            states::State::Context(m_render_window)
        )
    {
        m_render_window.setVerticalSyncEnabled(true);
        register_states();
    }

    int Application::run()
    {
        sf::Clock clock;
        sf::Time time_since_last_update = sf::Time::Zero;

        while(m_render_window.isOpen())
        {
            process_input();
            time_since_last_update += clock.restart();

            while(time_since_last_update > TIME_PER_FRAME) {
                time_since_last_update -= TIME_PER_FRAME;
                process_input();
                update(TIME_PER_FRAME);
            }

            draw();
        }

        return 0;
    }

    void Application::update(sf::Time dt)
    {
        m_state_stack.update(dt);
    }

    void Application::draw()
    {
        m_render_window.clear();
        m_state_stack.draw();
        m_render_window.display();
    }

    void Application::process_input()
    {
        sf::Event event;
        while(m_render_window.pollEvent(event)) {
            m_state_stack.handle_event(event); 
        }
    }

    void Application::register_states()
    {
           
    }
}
