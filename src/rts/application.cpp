#include "application.hpp"
#include "rts/states/loading_state.hpp"
#include "rts/states/main_menu.hpp"
#include "rts/states/title_state.hpp"
#include "rts/states/game_state.hpp"

#include <SFML/Window/Event.hpp>

namespace rts
{
    namespace
    {
        const sf::Time TIME_PER_FRAME = sf::seconds(1.0 / 50.0);

        const int MAX_FRAME_SKIPS = 5;
    }

    Application::Application():
        m_sfgui(),
        m_desktop(),
        m_render_window(sf::VideoMode(800, 600), "rts"),
        m_texture_holder(),
        m_font_holder(),
        m_shader_holder(),
        m_sound_holder(),
        m_music_holder(),
        m_focused(true),
        m_state_stack(states::State::Context(m_render_window, m_texture_holder, m_font_holder, m_shader_holder, m_sound_holder, m_music_holder, m_desktop, m_focused))
    {
        m_render_window.setVerticalSyncEnabled(true);
        m_render_window.resetGLStates();
        register_states();
    }

    int Application::run()
    {
        init();

        sf::Clock clock;
        sf::Time time_since_last_update = sf::Time::Zero;

        m_state_stack.push_state(states::ID::LoadingState);
        m_state_stack.update(TIME_PER_FRAME);

        while(m_render_window.isOpen())
        {
            process_input();
            time_since_last_update += clock.restart();

            int skips = 0;

            while(time_since_last_update > TIME_PER_FRAME && skips <= MAX_FRAME_SKIPS) {
                time_since_last_update -= TIME_PER_FRAME;
                process_input();
                update(TIME_PER_FRAME);
                skips++;
            }

            draw();

            if(m_state_stack.is_empty())
                m_render_window.close();
        }

        return 0;
    }

    void Application::init()
    {
    }

    void Application::update(sf::Time dt)
    {
        m_state_stack.update(dt);
    }

    void Application::draw()
    {
        m_render_window.clear();
        m_state_stack.draw();
        m_sfgui.Display(m_render_window);
        m_render_window.display();
    }

    void Application::process_input()
    {
        sf::Event event;
        while(m_render_window.pollEvent(event)) {
            switch(event.type) {
                case sf::Event::LostFocus:
                    m_focused = false;
                    break;
                case sf::Event::GainedFocus:
                    m_focused = true;
                    break;
                default: 
                    m_state_stack.handle_event(event); 
            }
        }
    }

    void Application::register_states()
    {
        m_state_stack.register_state<states::TitleState>(states::ID::TitleState);
        m_state_stack.register_state<states::LoadingState>(states::ID::LoadingState);
        m_state_stack.register_state<states::MainMenuState>(states::ID::MainMenuState);
        m_state_stack.register_state<states::GameState>(states::ID::GameState);
    }
}
