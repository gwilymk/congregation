print "Loading resources"

import load_texture, load_font from rts

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
            else
                print "Error, unknown type #{tbl.type}"
                error 1

    data()

resource_files = {
    "textures/title.lua"
    "fonts/font.lua",
}

for res in *resource_files
    load_resource(res)
