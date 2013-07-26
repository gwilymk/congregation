print "Loading resources"

import load_texture, load_font, load_shader from rts

export resource = (tbl) ->
    print "Loading #{tbl.name} (#{tbl.filename})"
    switch tbl.type
        when "font"
            load_font tbl.name, "../res/" .. tbl.filename
        when "texture"
            load_texture tbl.name, "../res/" .. tbl.filename, tbl.smooth
        when "shader"
            load_shader tbl.name, "../res/" .. tbl.vertex, "../res/" .. tbl.fragment
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
    "shaders/minion.lua"
    "textures/hats.lua"
    "textures/hud_background.lua"
    "textures/minion_counter.lua"
    "textures/minion.lua"
    "textures/tiles.lua"
    "textures/select_arrow.lua"
    "textures/title.lua"
    "textures/tile_placement_box.lua"
}

for res in *resource_files
    load_resource(res)
