#version 120
//Based on Ozone3D Phong + Materials	
	
varying vec3 normal, spotLightDir, pointLightDir; 
varying vec3 dirLightDir, eyeVec, reflecVec;

void main()
{	
	normal = gl_NormalMatrix * gl_Normal;

	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

	spotLightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
	pointLightDir = vec3(gl_LightSource[1].position.xyz - vVertex);
	dirLightDir = vec3(gl_LightSource[2].position.xyz);// - vVertex);
	
	eyeVec = -vVertex;

	gl_Position = ftransform();		
	
	//Add Texture
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	//Calculate reflection off vert
	vec3 NN = normalize(normal.xyz);
	reflecVec = reflect(eyeVec, NN);
}