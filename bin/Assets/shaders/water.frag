float CalculateFog();
float CalculateFog()
{	
	const float LOG2E = 1.442695;
	float fog = exp2(-gl_Fog.density * gl_FogFragCoord * LOG2E);
	//fog = clamp(fog, 0.0, 1.0);	
	return fog;	
}

uniform sampler2D waterTex;
uniform sampler2D normalTex;
uniform sampler2D DUDVTex;
uniform sampler2D skyMapTex;

varying vec4 projCoords;
varying vec3 lightDir, eyeDir;
varying vec2 flowCoords, rippleCoords;

const int STAR_LIGHT 	= 3;
const float f_TRANSPARENCY = 0.65;

void main()
{
   vec2 rippleEffect = 0.02 * texture2D(DUDVTex, rippleCoords * 0.1).xy;
   vec4 N = texture2D(normalTex, flowCoords + rippleEffect);
   N = N * 2.0 - vec4(1.0);
   N.a = 1.0; N = normalize(N);

   vec3 refVec = normalize(reflect(-lightDir, vec3(N) * 0.6));
   float refAngle = clamp(dot(eyeDir, refVec), 0.0, 1.0);
   //vec4 specular = vec4(pow(refAngle, 40.0));
   vec4 specular = vec4(pow(refAngle, 2.0));

   vec2 dist = texture2D(DUDVTex, flowCoords + rippleEffect).xy;
   dist = (dist * 2.0 - vec2(1.0)) * 0.1;
   vec2 uv = projCoords.xy / projCoords.w;
   uv = clamp((uv + 1.0) * 0.5 + dist, 0.0, 1.0);

   vec4 base = texture2D(skyMapTex, uv);
   
   vec4 tot = mix(base, specular, 0.6);
   vec4 wat = texture2D(waterTex, uv);
   vec4 finalColor = mix(tot, wat, 0.6);

   finalColor = vec4(finalColor.x, finalColor.y, finalColor.z, f_TRANSPARENCY);
      
	//Calcula la "luminosidad" de esta forma. Esta luminosidad es para saber cuanta luz tiene, y que por la noche no se vea tan resplandeciente
	float value = ( (gl_LightSource[STAR_LIGHT].diffuse.x) + (gl_LightSource[STAR_LIGHT].diffuse.x) + (gl_LightSource[STAR_LIGHT].diffuse.z) ) / 3;
	finalColor.x *= (value);
	finalColor.y *= (value);
	finalColor.z *= (value);
	
	//calculate fog
	float fog = CalculateFog();	
	if (fog >= 0.0)
	{
		finalColor = mix(gl_Fog.color, finalColor, fog);
	}

   gl_FragColor = finalColor;
}