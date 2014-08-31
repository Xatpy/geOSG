varying vec3 normal, lightDir, eyeVec;

const float cos_outer_cone_angle = 0.8; // 36 degrees

uniform sampler2D myTexture;
varying vec2 vTexCoord;

const int LIGHT = 4;

void main (void)
{
	vec4 final_color = (gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + (gl_LightSource[LIGHT].ambient * gl_FrontMaterial.ambient);

	vec3 L = normalize(lightDir);
	vec3 D = normalize(gl_LightSource[LIGHT].spotDirection);

	float cos_cur_angle = dot(-L, D);
	float cos_inner_cone_angle = gl_LightSource[LIGHT].spotCosCutoff;
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
		final_color += gl_LightSource[LIGHT].diffuse * gl_FrontMaterial.diffuse * lambertTerm * spot;

		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);

		float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );

		final_color += gl_LightSource[LIGHT].specular *	gl_FrontMaterial.specular * specular * spot;
		
		vec4 texturing = texture2D(myTexture, vTexCoord);
		if (spot >= 0.9)
		{			
			final_color += texturing;
		}
		else if ( (spot < 0.9) && (spot > 0.01) )
		{
			final_color = mix(texturing/10.0, final_color+texturing, spot);
		}
		else
		{
			final_color = (texturing / 10.0f);
		}
		
		gl_FragColor = final_color;
	}
}