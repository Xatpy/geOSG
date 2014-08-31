float GetFogFragCoord();
float GetFogFragCoord()
{
	vec4 eyeCoordPos = gl_ModelViewMatrix * gl_Vertex;
    return abs(eyeCoordPos.z/eyeCoordPos.w);
}

uniform float osg_FrameTime;

varying vec4 projCoords;
varying vec3 lightDir, eyeDir;
varying vec2 flowCoords, rippleCoords;

attribute vec3 rm_Binormal;
attribute vec3 rm_Tangent;

const int STAR_LIGHT 	= 3;

void main()
{
	vec3 N = gl_NormalMatrix * gl_Normal;
	vec3 B = gl_NormalMatrix * rm_Binormal;
	vec3 T = gl_NormalMatrix * rm_Tangent;
	
	T = normalize(gl_NormalMatrix * T);
	B = normalize(gl_NormalMatrix * B);
	N = normalize(gl_NormalMatrix * N);
	
	mat3 TBNmat;
	TBNmat[0][0] = T[0]; TBNmat[1][0] = T[1]; TBNmat[2][0] = T[2];
	TBNmat[0][1] = B[0]; TBNmat[1][1] = B[1]; TBNmat[2][1] = B[2];
	TBNmat[0][2] = N[0]; TBNmat[1][2] = N[1]; TBNmat[2][2] = N[2];
	
	vec3 vertexInEye = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir =  gl_LightSource[STAR_LIGHT].position.xyz - vertexInEye;
	lightDir = normalize(TBNmat * lightDir);
	eyeDir = normalize(TBNmat * (-vertexInEye));
	
	vec2 t1 = vec2(osg_FrameTime*0.02, osg_FrameTime*0.02);
	vec2 t2 = vec2(osg_FrameTime*0.05, osg_FrameTime*0.05);
	flowCoords = gl_MultiTexCoord0.xy * 5.0 + t1;
	rippleCoords = gl_MultiTexCoord0.xy * 10.0 + t2;
	/*vec2 t1 = vec2(osg_FrameTime*0.2, osg_FrameTime*0.2);
	vec2 t2 = vec2(osg_FrameTime*0.5, osg_FrameTime*0.5);
	flowCoords = gl_MultiTexCoord0.xy * 2.0 + t1;
	rippleCoords = gl_MultiTexCoord0.xy * 20.0 + t2;*/
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
   
	//------>>> ftransform() = gl_ModelViewProjectionMatrix * gl_Vertex 
	vec4 pos = gl_Vertex;
	float random = clamp(osg_FrameTime, 0.1, 0.5);
	pos.z += sin(pos.z + osg_FrameTime * random) * 0.05; 
	gl_Position = gl_ModelViewProjectionMatrix * pos; //gl_Position = ftransform();
	projCoords = gl_Position;
	
	gl_FogFragCoord = GetFogFragCoord();
}