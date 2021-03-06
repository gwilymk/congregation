/*
    Congregation - An rts game involving the placement of tiles
    Copyright (C) 2013  Gwilym Kuiper (kuiper.gwilym@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "state_stack.hpp"

#include "rts/common.hpp"

namespace rts
{
    StateStack::PendingChange::PendingChange(Action action, states::ID stateID):
        action(action), stateID(stateID)
    {
    }

    StateStack::~StateStack()
    {
        while(!is_empty())
            m_stack.pop_back();
    }

    states::State::ptr StateStack::create_state(states::ID stateID)
    {
        auto found = m_factories.find(stateID);
        ASSERT(found != m_factories.end());

        return found->second();
    }

    StateStack::StateStack(states::State::Context context):
        m_context(context)
    {
    }

    void StateStack::handle_event(const sf::Event &event)
    {
        for(auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
            if(!(*itr)->handle_event(event))
                return;
    }

    void StateStack::update(sf::Time dt)
    {
        for(auto itr = m_stack.rbegin(); itr != m_stack.rend(); ++itr)
            if(!(*itr)->update(dt))
                return;

        apply_pending_changes();
    }

    void StateStack::draw()
    {
        for(auto itr = m_stack.begin(); itr != m_stack.end(); ++itr)
            (*itr)->draw();
    }

    bool StateStack::is_empty()
    {
        return m_stack.empty();
    }

    void StateStack::apply_pending_changes()
    {
        for(auto change : m_pending_list) {
            switch(change.action) {
                case Action::Push:
                    m_stack.push_back(create_state(change.stateID));
                    break;
                case Action::Pop:
                    m_stack.pop_back();
                    break;
                case Action::Clear:
                    m_stack.clear();
                    break;
            }
        }

        m_pending_list.clear();
    }

    void StateStack::push_state(states::ID stateID)
    {
        m_pending_list.emplace_back(Action::Push, stateID);
    }

    void StateStack::pop_state()
    {
        m_pending_list.emplace_back(Action::Pop);
    }

    void StateStack::clear_states()
    {
        m_pending_list.emplace_back(Action::Clear);
    }
}
