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

#ifndef HOLDER_HPP
#define HOLDER_HPP

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <string>
#include <unordered_map>
#include <memory>

namespace rts
{
    namespace holders
    {
        template <class Resource> 
        class Holder : private sf::NonCopyable
        {
            public:
                Resource &get(const std::string &name);
                const Resource &get(const std::string &name) const;

                std::string get_fname(const std::string &name) const;

                template <typename Parameter>
                void load(const std::string &name, const std::string &filename, const Parameter &second_param);
                void load(const std::string &name, const std::string &filename);

            private:
                std::unordered_map<std::string, std::unique_ptr<Resource>> m_resource_map;
                std::unordered_map<std::string, std::string> m_fname_map;
        };

        typedef Holder<sf::Texture> TextureHolder;
        typedef Holder<sf::Font> FontHolder;
        typedef Holder<sf::Shader> ShaderHolder;
        typedef Holder<sf::SoundBuffer> SoundHolder;
    }
}

#include "holder.inl"

#endif
