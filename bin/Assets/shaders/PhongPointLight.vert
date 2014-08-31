varying vec3 normal, lightDir, eyeVec;
varying float att;

varying vec2 vTexCoord;  

varying vec3 lightVec;	/*!< Light vector passed to the fragment program.*/ 
varying vec3 viewVec;	/*!< View vector passed to the fragment program.*/
//varying vec2 texCoord;	/*!< Texture coordinates passed to the fragment program.*/

attribute vec3 Tangent;/*!< Tangent space vector.*/ 

const int LIGHT = 4;

void main()
{	
	normal = gl_NormalMatrix * gl_Normal;

	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = vec3(gl_LightSource[LIGHT].position.xyz - vVertex);
	eyeVec = -vVertex;
	
	float d = length(lightDir);	
	//att = 1.0 / ( gl_LightSource[0].constantAttenuation + (gl_LightSource[0].linearAttenuation * d) + (gl_LightSource[0].quadraticAttenuation * d *d) );	
	att = 1.0 / ( gl_LightSource[LIGHT].constantAttenuation + (gl_LightSource[LIGHT].linearAttenuation * d) + (gl_LightSource[LIGHT].quadraticAttenuation * d *d) );	

	vTexCoord = vec2(gl_MultiTexCoord0);
	gl_Position = ftransform();		
		
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * Tangent);
	vec3 b = cross(n, t);
		
	vec3 v;
	//vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	vec3 lVec = gl_LightSource[LIGHT].position.xyz - vVertex;
	
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