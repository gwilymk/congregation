--
--    Congregation - An rts game involving the placement of tiles
--    Copyright (C) 2013  Gwilym Kuiper (kuiper.gwilym@gmail.com)
--
--    This program is free software: you can redistribute it and/or modify
--    it under the terms of the GNU General Public License as published by
--    the Free Software Foundation, either version 3 of the License, or
--    (at your option) any later version.
--
--    This program is distributed in the hope that it will be useful,
--    but WITHOUT ANY WARRANTY; without even the implied warranty of
--    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--    GNU General Public License for more details.
--
--    You should have received a copy of the GNU General Public License
--    along with this program.  If not, see <http://www.gnu.org/licenses/>.
--

print "Loading resources"

import load_texture, load_font, load_shader, load_music, load_sound from rts

export resource = (tbl) ->
    print "Loading #{tbl.name} (#{tbl.filename})"
    switch tbl.type
        when "font"
            load_font tbl.name, "../res/" .. tbl.filename
        when "texture"
            load_texture tbl.name, "../res/" .. tbl.filename, tbl.smooth
        when "shader"
            load_shader tbl.name, "../res/" .. tbl.vertex, "../res/" .. tbl.fragment
        when "music"
            load_music tbl.name, "../res/" .. tbl.filename
        when "sound"
            load_sound tbl.name, "../res/" .. tbl.filename
        else
            print "Error, unknown type #{tbl.type}"
            error 1

load_resource = (filename) ->
    data = loadfile("../res/" .. filename)
    
    unless data
        print "Failed to load file " .. filename
        error 1

    data()

resource_files = {
    "fonts/font.lua"
    "music/theme_music.lua"
    "shaders/minion.lua"
    "textures/hats.lua"
    "textures/hud_background.lua"
    "textures/minion_counter.lua"
    "textures/minion.lua"
    "textures/select_arrow.lua"
    "textures/tile_placement_box.lua"
    "textures/tiles.lua"
    "textures/title.lua"
}

for res in *resource_files
    load_resource(res)
