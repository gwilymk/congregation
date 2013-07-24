#include "lobby.hpp"
#include "rts/common.hpp"

#include <SFGUI/SFGUI.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <sstream>

#include <iostream>

namespace
{
    const std::string has_server_message = "server?";
    const std::string reply = "me!";
    const unsigned short broadcast_port = 10037;

    template <typename T>
    std::string number_to_string(T Number)
    {
        std::ostringstream ss;
        ss << Number;
        return ss.str();
    }
}

namespace rts
{
    namespace states
    {
        namespace game
        {
            static sfg::Box::Ptr create_labeled_item(const std::string &label, const sfg::Widget::Ptr &widget)
            {
                sfg::Box::Ptr ret = sfg::Box::Create(sfg::Box::HORIZONTAL);
                ret->Pack(sfg::Label::Create(label));
                ret->Pack(widget);
                return ret;
            }

            struct Lobby::Impl
            {
                sfg::Window::Ptr window;
                sfg::Box::Ptr server_list;

                sfg::ComboBox::Ptr num_players;
                sfg::ComboBox::Ptr map_size;

                sfg::Spinner::Ptr update_spinner;

                sf::UdpSocket broadcast_socket;
                sfg::Desktop *desktop;
            };

            Lobby::Lobby(bool *done):
                m_done(done),
                m_impl(new Impl),
                server(false)
            {
                *m_done = false;
                m_impl->broadcast_socket.setBlocking(false);
            }

            Lobby::~Lobby()
            {}

            void Lobby::add_to_desktop(sfg::Desktop &desktop)
            {
                *m_done = false;
                m_impl->window = sfg::Window::Create();
                m_impl->window->SetRequisition(sf::Vector2f(300, 300));
                m_impl->window->SetTitle("Lobby");
                sfg::Notebook::Ptr notebook = sfg::Notebook::Create();
                m_impl->window->Add(notebook);
                m_impl->server_list = sfg::Box::Create(sfg::Box::VERTICAL);
                sfg::ScrolledWindow::Ptr scrolled = sfg::ScrolledWindow::Create();
                scrolled->Add(m_impl->server_list);
                scrolled->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_NEVER | sfg::ScrolledWindow::VERTICAL_ALWAYS);
                sfg::Box::Ptr vbox = sfg::Box::Create(sfg::Box::VERTICAL, 2.0f);
                sfg::Box::Ptr hbox = sfg::Box::Create(sfg::Box::HORIZONTAL, 2.0f);

                m_impl->update_spinner = sfg::Spinner::Create();
                sfg::Button::Ptr update = sfg::Button::Create("Update Server List");
                hbox->Pack(m_impl->update_spinner, true, true);
                hbox->Pack(update, true, true);
                update->GetSignal(sfg::Button::OnLeftClick).Connect(&Lobby::update_server_list, this);
                vbox->Pack(scrolled, true, true);
                vbox->Pack(hbox, false, false);
                notebook->AppendPage(vbox, sfg::Label::Create("Existing Servers"));

                vbox = sfg::Box::Create(sfg::Box::VERTICAL);

                m_impl->num_players = sfg::ComboBox::Create();
                vbox->Pack(create_labeled_item("Max Players", m_impl->num_players), false, false);
                m_impl->map_size = sfg::ComboBox::Create();
                vbox->Pack(create_labeled_item("Map Size", m_impl->map_size), false, false);

                sfg::Button::Ptr create = sfg::Button::Create("Create Server");
                create->GetSignal(sfg::Button::OnLeftClick).Connect(&Lobby::create_server, this);
                vbox->Pack(create, false, true);

                m_impl->map_size->AppendItem("Small");
                m_impl->map_size->AppendItem("Medium");
                m_impl->map_size->AppendItem("Large");

                for(int i = 2 ; i <= 8 ; ++i) {
                    m_impl->num_players->AppendItem(number_to_string(i));
                }

                notebook->AppendPage(vbox, sfg::Label::Create("Create Server"));

                m_impl->desktop = &desktop;
                desktop.Add(m_impl->window);
            }

            void Lobby::update_server_list()
            {
                assert(!*m_done);

                sf::Packet packet;
                packet << has_server_message;
                m_impl->broadcast_socket.send(packet, sf::IpAddress::Broadcast, broadcast_port);
            }

            void Lobby::create_server()
            {
                assert(!*m_done);
                server = true;
                m_impl->broadcast_socket.bind(broadcast_port);
            }

            void Lobby::update()
            {
                sf::Packet packet;
                sf::IpAddress addr;
                unsigned short port;
                if(m_impl->broadcast_socket.receive(packet, addr, port) == sf::Socket::Done) {
                    std::string message;
                    packet >> message;
                    if(message == has_server_message)
                        std::cerr << "Yay! " << addr.toString() << ":" << port << std::endl;
                }
            }
        }
    }
}
