varying vec3 normal, lightDir, eyeVec;
varying float att;

varying vec2 vTexCoord;  

varying vec3 lightVec;	
varying vec3 lightVecMoon;
varying vec3 viewVec;	

attribute vec3 Tangent;
		
const int SPOT_LIGHT_1 	= 4;

float GetFogFragCoord();
float GetFogFragCoord()
{
	vec4 eyeCoordPos = gl_ModelViewMatrix * gl_Vertex;
    return abs(eyeCoordPos.z/eyeCoordPos.w);
}

void main()
{	
	gl_Position = ftransform();		
	
	normal = gl_NormalMatrix * gl_Normal;

	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = vec3(gl_LightSource[SPOT_LIGHT_1].position.xyz - vVertex);
	eyeVec = -vVertex;
	
	float d = length(lightDir);	
	att = 1.0 / ( gl_LightSource[SPOT_LIGHT_1].constantAttenuation + (gl_LightSource[SPOT_LIGHT_1].linearAttenuation * d) + (gl_LightSource[SPOT_LIGHT_1].quadraticAttenuation * d *d) );	

	vTexCoord = vec2(gl_MultiTexCoord0);
		
	vec3 n = normalize(gl_NormalMatrix * gl_Normal);
	vec3 t = normalize(gl_NormalMatrix * Tangent);
	vec3 b = cross(n, t);
	
	vec3 v;	
	vec3 lVecMoon = gl_LightSource[SPOT_LIGHT_1].position.xyz - vVertex;	 //Star light
	v.x = dot(lVecMoon, t);
	v.y = dot(lVecMoon, b);
	v.z = dot(lVecMoon, n);
	lightVecMoon = v;
	
	vec3 lVec = gl_LightSource[SPOT_LIGHT_1].position.xyz - vVertex;	//Spotlight 1
	v.x = dot(lVec, t);
	v.y = dot(lVec, b);
	v.z = dot(lVec, n);
	lightVec = v;
	
	vec3 vVec = -vVertex;
	v.x = dot(vVec, t);
	v.y = dot(-vVec, b);
	v.z = dot(vVec, n);
	viewVec = v;	
		
    gl_FogFragCoord = GetFogFragCoord();
}