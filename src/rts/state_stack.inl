namespace rts
{
    template <class T>
    void StateStack::register_state(states::ID stateID)
    {
        m_factories[stateID] = [this] ()
        {
            return states::State::ptr(new T(*this, m_context));
        };
    }
}
