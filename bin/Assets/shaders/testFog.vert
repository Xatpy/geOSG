float GetFogFragCoord();
float GetFogFragCoord()
{
	vec4 eyeCoordPos = gl_ModelViewMatrix * gl_Vertex;
    return abs(eyeCoordPos.z/eyeCoordPos.w);
}
		
void main()
{		
	gl_Position = ftransform();
	gl_FogFragCoord = GetFogFragCoord();
} 
