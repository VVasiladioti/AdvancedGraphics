#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../../nclgl/OBJmesh.h"`
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"

#define POST_PASSES 10

#define LIGHTNUM 16 //8

class RendererScene2 : public OGLRenderer {
public:
	RendererScene2(Window & parent);
	virtual ~RendererScene2(void);
	
	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:

	void PresentScene();
	void DrawPostProcess();
	//void DrawScene();

	void FillBuffers(); //G- Buffer Fill Render Pass
	void DrawPointLights(); // Lighting Render Pass
	void CombineBuffers(); // Combination Render Pass
	void GenerateScreenTexture(GLuint & into, bool depth = false);
	Shader * sceneShader; // check this
	Shader * processShader;
	Shader * pointlightShader; // Shader to calculate lighting
	Shader * combineShader; // shader to stick it all together

	Light * pointLights; // Array of lighting data
	Mesh * heightMap;
	OBJMesh * sphere; // Light volume
	Mesh * quad;
	Camera * camera;
	Light * light;

	float rotation; // How much to increase rotation by

	GLuint bufferFBO; //check this
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferColourTexture;
	GLuint bufferNormalTex; // Normals go here
	GLuint bufferDepthTex;

	GLuint pointLightFBO; // FBO for our lighting pass
	GLuint lightEmissiveTex; // Store emissive lighting
	GLuint lightSpecularTex;
	// Inherited via OGLRenderer
	// Store specular lighting
};