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
