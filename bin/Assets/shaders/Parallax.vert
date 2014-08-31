varying vec3 lightVec;	/*!< Light vector passed to the fragment program.*/ 
varying vec3 viewVec;	/*!< View vector passed to the fragment program.*/
varying vec2 texCoord;	/*!< Texture coordinates passed to the fragment program.*/

attribute vec3 Tangent;/*!< Tangent space vector.*/ 
	                
const int i_STAR_LIGHT = 4;					

void main(void)
{
	gl_Position = ftransform();
	texCoord = vec2(gl_MultiTexCoord0);		
		
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * Tangent);
	vec3 b = cross(n, t);
		
	vec3 v;
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	vec3 lVec = gl_LightSource[i_STAR_LIGHT].position.xyz - vVertex;
	
	v.x = dot(lVec, t);
	v.y = dot(lVec, b);
	v.z = dot(lVec, n);
	lightVec = v;
	
	vec3 vVec = -vVertex;
	v.x = dot(vVec, t);
	v.y = dot(-vVec, b);
	v.z = dot(vVec, n);
	viewVec = v;
}
