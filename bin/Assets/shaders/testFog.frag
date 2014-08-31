void main()
{
	const float LOG2E = 1.442695;
	float fog = exp2(-gl_Fog.density * gl_FogFragCoord * LOG2E);
	
	vec4 color = vec4(0,1,0,1);	
	if (fog >= 0.0)
		gl_FragColor = mix(gl_Fog.color,color, fog);
	else
		gl_FragColor = vec4(0,0,0,1);
}