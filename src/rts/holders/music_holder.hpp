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
                bool is_playing() const;

                float get_volume() const;
                void set_volume(float new_volume);
            
            private:
                sf::Music m_current_music;
                std::unordered_map<std::string, std::string> m_music_map;
        };
    }
}
#endif
