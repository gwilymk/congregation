#ifndef MUSIC_HOLDER_HPP
#define MUSIC_HOLDER_HPP

#include <string>
#include <unordered_map>

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Audio/Music.hpp>

namespace rts
{
    namespace holders
    {
        class MusicHolder : private sf::NonCopyable
        {
            public:
                void load(const std::string &name, const std::string &fname);

                void play(const std::string &name, bool loop = true);
                void stop();
            
            private:
                sf::Music m_current_music;
                std::unordered_map<std::string, std::string> m_music_map;
        };
    }
}
#endif
