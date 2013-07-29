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
