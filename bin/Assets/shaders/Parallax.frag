varying vec3 lightVec;/*!< Light vector passed from the vertex program.*/ 
varying vec3 viewVec;/*!< View vector passed from the vertex program.*/
varying vec2 texCoord;/*!< Texture coordinates passed from the vertex program.*/

uniform sampler2D colorMap;/*!< Diffuse texture sampler.*/
uniform sampler2D normalMap;/*!< Normal texture sampler.*/
uniform sampler2D heightMap;/*!< Height map sampler.*/

const int i_STAR_LIGHT = 4;		

void main (void)
{
	vec3 lVec = normalize(lightVec);
	vec3 vVec = normalize(viewVec);
		
	// Calculate offset, scale & bias
	float height = texture2D(heightMap, texCoord).x ;
	vec2 newTexCoord = texCoord + ((height * 0.04 - 0.02) * vVec.xy);		
		
	vec4 base = texture2D(colorMap, newTexCoord);
	vec3 bump = normalize(texture2D(normalMap, newTexCoord).xyz * 2.0 - 1.0);
	bump = normalize(bump);
	
	float diffuse = max( dot(lVec, bump), 0.0 );		
	float specular = pow(clamp(dot(reflect(-vVec, bump), lVec), 0.0, 1.0), gl_FrontMaterial.shininess );
	
	vec4 vAmbient 	=	gl_LightSource[i_STAR_LIGHT].ambient * gl_FrontMaterial.ambient;	
	vec4 vDiffuse 	=	gl_LightSource[i_STAR_LIGHT].diffuse * gl_FrontMaterial.diffuse * diffuse;	
	vec4 vSpecular 	=	gl_LightSource[i_STAR_LIGHT].specular * gl_FrontMaterial.specular *specular;	
	
	gl_FragColor = vAmbient*base + (vDiffuse*base + vSpecular);		
}
