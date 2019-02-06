#pragma once

#include "../../nclGL/OGLRenderer.h"
#include "../../nclGL/Camera.h"
#include "../../nclGL/SceneNode.h"
#include "../../nclGL/Frustum.h"
#include "CubeRobot.h"
#include <algorithm> // For std :: sort ...

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	virtual void UpdateScene(float msec);
	virtual void RenderScene();
	
protected:
	void BuildNodeLists(SceneNode * from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes();
	void DrawNode(SceneNode * n);
	
	SceneNode * root;
	Camera * camera;
	Mesh * quad;
	
	Frustum frameFrustum;
	
	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;
};

