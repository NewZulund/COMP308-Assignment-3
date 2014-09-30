#version 120

//Based on Ozone3D Phong 

varying vec3 normal, spotLightDir, pointLightDir;
varying vec3  dirLightDir, eyeVec, reflecVec;
varying vec2 texUV;

uniform sampler2D curTexture;
uniform samplerCube cubeMap;

void main (void)
{
	float specular = 0;
	float spotSpec = 0;
	float pointSpec = 0;
	
	//Includes ambient light
	vec4 final_color = (gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + (gl_LightSource[3].ambient * gl_FrontMaterial.ambient);
	vec4 cube_map_color = textureCube(cubeMap, reflecVec);
		
	vec3 N = normalize(normal);
	vec3 L = normalize(spotLightDir);
	vec3 D = normalize(gl_LightSource[0].spotDirection);
	float lambertTerm = dot(N,L);

	if (dot(-L, D) > gl_LightSource[0].spotCosCutoff) {
		if(lambertTerm > 0.0)
		{
			final_color += gl_LightSource[0].diffuse *  gl_FrontMaterial.diffuse * lambertTerm;	
			
			vec3 E = normalize(eyeVec);
			vec3 R = reflect(-L, N);
			float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
			final_color += gl_LightSource[0].specular *  gl_FrontMaterial.specular * specular;	
			
		}
	}
	
	//Point Light
	L = normalize(pointLightDir);
	lambertTerm = dot(N,L);
	if(lambertTerm > 0.0)
	{
		final_color += gl_LightSource[1].diffuse *  gl_FrontMaterial.diffuse *  lambertTerm;	
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
		final_color += gl_LightSource[1].specular * gl_FrontMaterial.specular * specular;	
	}

	//Direction Light
	L = normalize(pointLightDir);
	lambertTerm = dot(N,L);
	if(lambertTerm > 0.0)
	{
		final_color += gl_LightSource[2].diffuse *  gl_FrontMaterial.diffuse *  lambertTerm;	
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
		final_color += gl_LightSource[2].specular * gl_FrontMaterial.specular * specular;	
	}

	final_color *= texture2D(curTexture, gl_TexCoord[0].st); 
	
	//CubeMap multiplication
	final_color = vec4(cube_map_color.xyz, 1.0);
	gl_FragColor = final_color;
			
	
}
