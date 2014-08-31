varying vec3 N;
varying vec3 v; 
varying vec2 vTexCoord;    

uniform sampler2D myTexture;

const int LIGHT = 4;

void main (void)  
{  
	vec3 L = normalize(gl_LightSource[LIGHT].position.xyz - v);   

    //calculate Diffuse Term:  
    vec4 Idiff = gl_FrontLightProduct[LIGHT].diffuse * max(dot(N,L), 0.0);    	
	Idiff *= 0.3f; //le bajo el valor para que sea como más "translúcido" y que la luz no queme tanto
	
	gl_FragColor = (texture2D(myTexture, vTexCoord)) + Idiff;	
 }