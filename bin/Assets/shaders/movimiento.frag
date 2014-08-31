uniform sampler2D tex;
uniform sampler2D tex2;

void main()
{
	vec4 value1 = texture2D(tex, gl_TexCoord[0].st) * 0.9;
	vec4 value2 = texture2D(tex2, gl_TexCoord[1].st) * 0.5;
	gl_FragColor = value1 + value2;
}
