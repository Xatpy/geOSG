varying vec3 normal, lightDir, lightDir2, eyeVec;

const float cos_outer_cone_angle = 0.8; // 36 degrees

varying vec2 vTexCoord;

//---------------------PARALLAX----------------
varying vec3 lightVec;
varying vec3 lightVec2;
varying vec3 lightVecMoon;
varying vec3 viewVec;

uniform sampler2D colorMap;/*!< Diffuse texture sampler.*/
uniform sampler2D normalMap;/*!< Normal texture sampler.*/
uniform sampler2D heightMap;/*!< Height map sampler.*/
uniform sampler2D specularMap;/*!< Height map sampler.*/
uniform sampler2D opacityMap;/*!< Height map sampler.*/

uniform int iTypeLight;

const int STAR_LIGHT 	= 3;			
const int SPOT_LIGHT_1	= 4;			
const int SPOT_LIGHT_2	= 5;	

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
	vec3 lVecMoon = normalize(lightVecMoon);
	vec3 vVecMoon = normalize(viewVec);
		
	// Calculate offset, scale & bias
	float heightMoon = texture2D(heightMap, vTexCoord).x ;
	vec2 newTexCoordMoon = vTexCoord + ((heightMoon * 0.04 - 0.02) * vVecMoon.xy);		
		
	vec4 baseMoon = texture2D(colorMap, newTexCoordMoon);
	vec3 bumpMoon = normalize(texture2D(normalMap, newTexCoordMoon).xyz * 2.0 - 1.0);
	bumpMoon = normalize(bumpMoon);
	
	float diffuseMoon = max( dot(lVecMoon, bumpMoon), 0.0 );		
	float specularMoon = pow(clamp(dot(reflect(-vVecMoon, bumpMoon), lVecMoon), 0.0, 1.0), gl_FrontMaterial.shininess );
	
	vec4 vAmbientMoon = gl_LightSource[STAR_LIGHT].ambient * gl_FrontMaterial.ambient;	
	vec4 vDiffuseMoon = gl_LightSource[STAR_LIGHT].diffuse * gl_FrontMaterial.diffuse * diffuseMoon;	
	vec4 vSpecularMoon = gl_LightSource[STAR_LIGHT].specular * gl_FrontMaterial.specular *specularMoon;	

	vec4 moonColor = vAmbientMoon * baseMoon + (vDiffuseMoon * baseMoon + vSpecularMoon);
	moonColor /= 3.0;
	
	vec4 finalOutput = vec4(0,0,0,0);
	if (iTypeLight == 1)
	{
		//------------------------------------spot light----------------------------------------------		
		vec3 vVec = normalize(viewVec);
		// Calculate offset, scale & bias
		float height = texture2D(heightMap, vTexCoord).x ;
		vec2 newTexCoord = vTexCoord + ((height * 0.04 - 0.02) * vVec.xy);	
								
		vec4 base = texture2D(colorMap, newTexCoord);
		vec3 bump = normalize(texture2D(normalMap, newTexCoord).xyz * 2.0 - 1.0);
		bump = normalize(bump);
		
		vec4 vecEspeculando = texture2D(specularMap, vTexCoord) ;   
		float valueSpecular = vecEspeculando.x + vecEspeculando.y + vecEspeculando.z;
		
		vec3 E = normalize(eyeVec);		
		
		for (int i = 0; i < 2; ++i) //light loops
		{		
			vec3 lVec;
			if (i == 0)
				lVec = normalize(lightVec);
			else
				lVec = normalize(lightVec2);									
			
			float diffuse = max( dot(lVec, bump), 0.0 );		
			float specular = pow(clamp(dot(reflect(-vVec, bump), lVec), 0.0, 1.0), gl_FrontMaterial.shininess );
			
			vec4 vAmbient, vDiffuse, vSpecular;			
			if (i == 0)
			{
				vAmbient = gl_LightSource[SPOT_LIGHT_1].ambient * gl_FrontMaterial.ambient;	
				vDiffuse = gl_LightSource[SPOT_LIGHT_1].diffuse * gl_FrontMaterial.diffuse * diffuse;	
				vSpecular = gl_LightSource[SPOT_LIGHT_1].specular * gl_FrontMaterial.specular * specular;	
			}
			else
			{
				vAmbient = gl_LightSource[SPOT_LIGHT_2].ambient * gl_FrontMaterial.ambient;	
				vDiffuse = gl_LightSource[SPOT_LIGHT_2].diffuse * gl_FrontMaterial.diffuse * diffuse;	
				vSpecular = gl_LightSource[SPOT_LIGHT_2].specular * gl_FrontMaterial.specular * specular;	
			}			
			
			vec4 outParallax = vAmbient * base + vDiffuse * base + (vSpecular * valueSpecular);
			
			//----------------------------------------SPOT-----------------------------------------------------------------------
			vec4 final_color = (gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + (gl_LightSource[1].ambient * gl_FrontMaterial.ambient);	//LUNA

			vec3 L;//vec3 L = normalize(lightDir);
			vec3 D; //vec3 D = normalize(gl_LightSource[4].spotDirection);
			if (i == 0)
			{	
				L = normalize(lightDir);
				D = normalize(gl_LightSource[SPOT_LIGHT_1].spotDirection);				
			}
			else
			{
				L = normalize(lightDir2);
				D = normalize(gl_LightSource[SPOT_LIGHT_2].spotDirection);							
			}

			float cos_cur_angle = dot(-L, D);			
			float cos_inner_cone_angle;//float cos_inner_cone_angle = gl_LightSource[4].spotCosCutoff;
			if (i == 0)
				cos_inner_cone_angle = gl_LightSource[SPOT_LIGHT_1].spotCosCutoff;
			else
				cos_inner_cone_angle = gl_LightSource[SPOT_LIGHT_2].spotCosCutoff;
				
			float cos_inner_minus_outer_angle = cos_inner_cone_angle - cos_outer_cone_angle;
			
			//****************************************************
			// Don't need dynamic branching at all, precompute  falloff(i will call it spot)
			float spot = 0.0;
			spot = clamp((cos_cur_angle - cos_outer_cone_angle) / cos_inner_minus_outer_angle, 0.0, 1.0);
			//****************************************************

			vec3 N = normalize(normal);

			float lambertTerm = max( dot(N,L), 0.0);
			if(lambertTerm > 0.0)
			{
				if (i == 0)
					final_color += gl_LightSource[SPOT_LIGHT_1].diffuse * gl_FrontMaterial.diffuse * lambertTerm * spot;
				else
					final_color += gl_LightSource[SPOT_LIGHT_2].diffuse * gl_FrontMaterial.diffuse * lambertTerm * spot;
									
				vec3 R = reflect(-L, N);
				float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
				
				if (i == 0)
					final_color += gl_LightSource[SPOT_LIGHT_1].specular *	gl_FrontMaterial.specular * specular * spot;
				else
					final_color += gl_LightSource[SPOT_LIGHT_2].specular *	gl_FrontMaterial.specular * specular * spot;
								
				if (spot >= 0.9)
				{			
					final_color = outParallax;
				}
				else if ( (spot < 0.9) && (spot > 0.01) )
				{
					final_color = mix(moonColor, outParallax, spot);
				}
				else
				{			
					final_color = moonColor;
				}
				finalOutput += final_color;
			}
			else
			{
				finalOutput += moonColor;
			}		
		}//light loops
		//gl_FragColor = output;				
	}
	else //if iTypeLight == 0
	{
		finalOutput = moonColor;
	}
	
	//Calculate opacity
	vec4 vecOpacity = texture2D(opacityMap, vTexCoord);		
	finalOutput = vec4(finalOutput.x,finalOutput.y,finalOutput.z, (vecOpacity.x) );				
	
	//calculate fog
	float fog = CalculateFog();	
	if (fog >= 0.0)
	{
		finalOutput = mix(gl_Fog.color, finalOutput, fog);
	}
	
	gl_FragColor = finalOutput;	
}	