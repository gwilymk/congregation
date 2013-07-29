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

uniform sampler2D texture;

uniform vec4 minion_colour;

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    if(pixel.r != 0.0 && pixel.r == pixel.b && pixel.g == 0.0f && pixel.a != 0.0f)
        gl_FragColor = vec4(minion_colour.rgb * pixel.r, 1.0f);
    else
        gl_FragColor = pixel;
}
