//uniform sampler2D myTexture;
varying vec2 vTexCoord;
uniform vec4 vColor;

void main(void)
{
   //gl_FragColor = texture2D(myTexture, vTexCoord);
   gl_FragColor = vColor;
}