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
