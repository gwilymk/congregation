#include "lobby.hpp"
#include "rts/common.hpp"

#include <SFGUI/SFGUI.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <sstream>
#include <cstdlib>
#include <stdarg.h>

#include <iostream>

namespace
{
    const sf::Time time_to_wait = sf::seconds(5);

    template <typename T>
    std::string number_to_string(T Number)
    {
        std::ostringstream ss;
        ss << Number;
        return ss.str();
    }

    std::string string_format(const std::string fmt, ...) {
        int size = 100;
        std::string str;
        va_list ap;
        while (1) {
            str.resize(size);
            va_start(ap, fmt);
            int n = vsnprintf((char *)str.c_str(), size, fmt.c_str(), ap);
            va_end(ap);
            if (n > -1 && n < size) {
                str.resize(n);
                return str;
            }
            if (n > -1)
                size = n + 1;
            else
                size *= 2;
        }
        return str;
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
                sf::Time time_since_update;

                sf::UdpSocket broadcast_socket;
                sfg::Desktop *desktop;

                std::vector<std::pair<sf::IpAddress, unsigned short>> servers;
                std::vector<std::unique_ptr<ButtonCallback>> callbacks;

                network::ServerInfo info;
            };

            struct Lobby::ButtonCallback
            {
                ButtonCallback(Lobby *parent, int server):
                    parent(parent), server(server) {}

                void call()
                {
                    parent->set_server(server);
                    *parent->m_done = true;
                }

                Lobby *parent;
                int server;
            };

            Lobby::Lobby(bool *done):
                m_done(done),
                m_impl(new Impl),
                m_server(false),
                m_serverid(-1)
            {
                *m_done = false;
                m_impl->broadcast_socket.setBlocking(false);
            }

            Lobby::~Lobby()
            {
                m_impl->desktop->Remove(m_impl->window);
            }

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
                scrolled->AddWithViewport(m_impl->server_list);
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

                for(std::string name : network::sizes)
                    m_impl->map_size->AppendItem(name);

                for(int i = 2 ; i <= network::max_players ; ++i)
                    m_impl->num_players->AppendItem(number_to_string(i));

                notebook->AppendPage(vbox, sfg::Label::Create("Create Server"));

                m_impl->desktop = &desktop;
                desktop.Add(m_impl->window);
                update_server_list();
            }

            void Lobby::update_server_list()
            {
                assert(!*m_done);
                m_impl->server_list->RemoveAll();
                m_impl->servers.clear();
                m_impl->callbacks.clear();

                sf::Packet packet;
                packet << network::has_server_message;
                m_impl->broadcast_socket.send(packet, sf::IpAddress::Broadcast, network::broadcast_port);
                m_impl->update_spinner->Start();
                m_impl->time_since_update = sf::Time::Zero;
            }

            void Lobby::create_server()
            {
                assert(!*m_done);

                sf::String num_players_str = m_impl->num_players->GetSelectedText();
                if(num_players_str.isEmpty()) 
                    return;
                    
                int num_players = std::atoi(num_players_str.toAnsiString().c_str());
                m_impl->info.max_players = num_players;

                sf::String map_size_str = m_impl->map_size->GetSelectedText();
                if(map_size_str.isEmpty())
                    return;

                for(unsigned int i = 0; i < network::sizes.size(); ++i) {
                    if(network::sizes[i] == map_size_str) {
                        m_impl->info.map_size = i;
                        break;
                    } 
                }

                m_impl->info.num_players = 1;

                m_server = true;
                *m_done = true;
            }

            bool Lobby::server() const
            {
                return m_server;
            }

            network::ServerInfo Lobby::get_server_info() const
            {
                return m_impl->info;
            }

            void Lobby::update(sf::Time dt)
            {
                if(!m_server) {
                    client_update(dt); 
                }
            }

            void Lobby::client_update(sf::Time dt)
            {
                m_impl->time_since_update += dt;
                
                if(m_impl->time_since_update >= time_to_wait) {
                    m_impl->update_spinner->Stop();
                    return;
                }

                sf::Packet packet;
                sf::IpAddress addr;
                unsigned short port;

                if(m_impl->broadcast_socket.receive(packet, addr, port) == sf::Socket::Done) {
                    std::string message;
                    network::ServerInfo info;
                    packet >> message;
                    if(message == network::reply_message) {
                        sf::Uint16 tcp_port;
                        packet >> tcp_port;
                        std::cerr << "Found server: " << addr.toString() << ":" << tcp_port << std::endl;
                        sf::TcpSocket request_socket;

                        assert(request_socket.connect(addr, tcp_port) == sf::Socket::Done);
                        sf::Packet query;
                        query << network::want_info;
                        assert(request_socket.send(query) == sf::Socket::Done);
                        assert(request_socket.receive(packet) == sf::Socket::Done);
                        assert(packet >> info);
                        request_socket.disconnect();

                        m_impl->servers.push_back(std::make_pair(addr, tcp_port));
                        std::ostringstream ss;
                        ss << addr.toString() << ' ' << (unsigned) info.num_players << '/' << (unsigned) info.max_players << " (" << network::sizes.at(info.map_size) << ")";
                        sfg::Button::Ptr b = sfg::Button::Create(ss.str());
                        ButtonCallback *bc = new ButtonCallback(this, m_impl->servers.size() - 1);
                        m_impl->callbacks.emplace_back(bc);
                        b->GetSignal(sfg::Button::OnLeftClick).Connect(&ButtonCallback::call, bc);
                        m_impl->server_list->Pack(b, false);
                        m_impl->server_list->Refresh();
                    }
                }
            }

            sf::IpAddress Lobby::get_server() const
            {
                return m_impl->servers[m_serverid].first;
            }

            unsigned short Lobby::get_server_port() const
            {
                return m_impl->servers[m_serverid].second;
            }

            void Lobby::set_server(int serverid)
            {
                m_serverid = serverid;
            }
        }
    }
}
