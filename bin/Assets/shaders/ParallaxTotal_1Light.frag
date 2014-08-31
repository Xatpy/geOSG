varying vec3 normal, lightDir, eyeVec;

const float cos_outer_cone_angle = 0.8; // 36 degrees

varying vec2 vTexCoord;

varying vec3 lightVec;	
//varying vec3 lightVecMoon;
varying vec3 viewVec;

uniform sampler2D colorMap;/*!< Diffuse texture sampler.*/
uniform sampler2D normalMap;/*!< Normal texture sampler.*/
uniform sampler2D heightMap;/*!< Height map sampler.*/
uniform sampler2D specularMap;/*!< Height map sampler.*/

/*const int STAR_LIGHT 	= 1;			
const int SPOT_LIGHT_1	= 3;	//4		
const int SPOT_LIGHT_2	= 4;	*/

const int STAR_LIGHT 	= 3;			
const int SPOT_LIGHT_1	= 4;	//4		
//const int SPOT_LIGHT_2	= 4;	*/

float CalculateFog();
float CalculateFog()
{	
	const float LOG2E = 1.442695;
	float fog = exp2(-gl_Fog.density * gl_FogFragCoord * LOG2E);
	//fog = clamp(fog, 0.0, 1.0);	
	return fog;	
}

void main (void)
{
	vec3 lVec = normalize(lightVec);
	vec3 vVec = normalize(viewVec);
		
	// Calculate offset, scale & bias
	float height = texture2D(heightMap, vTexCoord).x ;
	vec2 newTexCoord = vTexCoord + ((height * 0.04 - 0.02) * vVec.xy);		
		
	vec4 base = texture2D(colorMap, newTexCoord);
	vec3 bump = normalize(texture2D(normalMap, newTexCoord).xyz * 2.0 - 1.0);
	bump = normalize(bump);
	
	float diffuse = max( dot(lVec, bump), 0.0 );		
	float specular = pow(clamp(dot(reflect(-vVec, bump), lVec), 0.0, 1.0), gl_FrontMaterial.shininess );
	
	vec4 vAmbient 	=	gl_LightSource[SPOT_LIGHT_1].ambient * gl_FrontMaterial.ambient;	
	vec4 vDiffuse 	=	gl_LightSource[SPOT_LIGHT_1].diffuse * gl_FrontMaterial.diffuse * diffuse;	
	vec4 vSpecular 	=	gl_LightSource[SPOT_LIGHT_1].specular * gl_FrontMaterial.specular * specular;	
	
	vec4 vecEspeculando = texture2D(specularMap, vTexCoord) ;   
	float valueSpecular = texture2D(specularMap, vTexCoord).x; // Greys --> x = y = z 
		
	gl_FragColor = vAmbient*base + (vDiffuse*base) + (vSpecular * valueSpecular);		
}	