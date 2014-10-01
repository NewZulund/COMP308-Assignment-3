#version 120

//Based on Ozone3D Phong 

varying vec3 normal, spotLightDir, pointLightDir;
varying vec3  dirLightDir, eyeVec, reflecVec;
varying vec2 texUV;

uniform samplerCube cubeMap;

void main (void)
{
	vec4 specColor = vec4(0.0f,0.0f,0.0f,1.0f);
	vec4 diffColor = vec4(0.0f,0.0f,0.0f,1.0f);
	vec4 ambiColor = (gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + (gl_LightSource[3].ambient * gl_FrontMaterial.ambient);
	vec4 cubeColor = vec4(textureCube(cubeMap, reflecVec).xyz,1.0f);
	
	vec3 N = normalize(normal);
	vec3 L = normalize(spotLightDir);
	vec3 D = normalize(gl_LightSource[0].spotDirection);
	float lambertTerm = dot(N,L);

	if (dot(-L, D) > gl_LightSource[0].spotCosCutoff) {
		if(lambertTerm > 0.0)
		{
			diffColor += gl_LightSource[0].diffuse *  gl_FrontMaterial.diffuse * lambertTerm;	
			
			vec3 E = normalize(eyeVec);
			vec3 R = reflect(-L, N);
			float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
			specColor += gl_LightSource[0].specular *  gl_FrontMaterial.specular * specular;	
			
		}
	}
	
	//Point Light
	L = normalize(pointLightDir);
	lambertTerm = dot(N,L);
	if(lambertTerm > 0.0)
	{
		diffColor += gl_LightSource[1].diffuse *  gl_FrontMaterial.diffuse *  lambertTerm;	
		
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
		specColor += gl_LightSource[1].specular * gl_FrontMaterial.specular * specular;	
	}

	//Direction Light
	L = normalize(dirLightDir);
	lambertTerm = dot(N,L);
	if(lambertTerm > 0.0)
	{
		diffColor += gl_LightSource[2].diffuse *  gl_FrontMaterial.diffuse *  lambertTerm;	
		vec3 E = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
		specColor += gl_LightSource[2].specular * gl_FrontMaterial.specular * specular;	
	}

	vec4 final_color = cubeColor * (diffColor + ambiColor + specColor);
	gl_FragColor = final_color;
			
}
