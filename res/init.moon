print "Loading resources"

import load_texture, load_font, load_shader from rts

load_resource = (filename) ->
    data = loadfile("../res/" .. filename)
    
    unless data
        print "Failed to load file " .. filename
        error 1

    export resource = (tbl) ->
        switch tbl.type
            when "font"
                load_font tbl.name, "../res/" .. tbl.filename
            when "texture"
                load_texture tbl.name, "../res/" .. tbl.filename
            when "shader"
                load_shader tbl.name, "../res/" .. tbl.vertex, "../res/" .. tbl.fragment
            else
                print "Error, unknown type #{tbl.type}"
                error 1

    data()

resource_files = {
    "textures/title.lua"
    "textures/tiles.lua"
    "textures/minion.lua"
    "textures/hats.lua"
    "fonts/font.lua"
    "shaders/minion.lua"
}

for res in *resource_files
    load_resource(res)
