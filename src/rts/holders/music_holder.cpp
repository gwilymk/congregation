#include "music_holder.hpp"
#include "rts/common.hpp"

namespace rts
{
    namespace holders
    {
        void MusicHolder::load(const std::string &name, const std::string &fname)
        {
            ASSERT(m_music_map.insert(std::make_pair(name, fname)).second == true);
        }

        void MusicHolder::play(const std::string &name, bool loop)
        {
            m_current_music.openFromFile(m_music_map.at(name));
            m_current_music.setLoop(loop);
            m_current_music.play();
        }

        void MusicHolder::stop()
        {
            m_current_music.stop();
        }
    }
}
