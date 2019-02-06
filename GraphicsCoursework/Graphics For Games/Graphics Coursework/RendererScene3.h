#pragma once

#include "../../nclGL/OGLRenderer.h"
#include "../../nclGL/Camera.h"
#include "../../nclGL/SceneNode.h"
#include "../../nclGL/Frustum.h"
#include "../../nclGL/CubeRobot.h"
#include <algorithm> // For std :: sort ...
#include "../../nclGL/ParticleEmitter.h"

class RendererScene3 : public OGLRenderer {
public:
	RendererScene3(Window & parent);
	virtual ~RendererScene3(void);
	virtual void UpdateScene(float msec);
	virtual void RenderScene();

protected:
	void BuildNodeLists(SceneNode * from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode * n);
	void DrawFloor();

	//

	void SetShaderParticleSize(float f);	//And a new setter

	SceneNode * root;
	Camera * camera;
	//Mesh * quad;
	Mesh * floor;

	Frustum frameFrustum;

	Shader * sceneShader;
	Shader * robotShader;
	Shader * geomShader;
	ParticleEmitter* emitter;	//A single particle emitter
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};
