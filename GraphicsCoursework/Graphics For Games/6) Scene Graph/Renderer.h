#pragma once

#include "../../nclGL/OGLRenderer.h"
#include "../../nclGL/Camera.h"
#include "../../nclGL/SceneNode.h"
#include "CubeRobot.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	
	virtual void UpdateScene(float msec);
	virtual void RenderScene();
	
protected:
	void DrawNode(SceneNode * n);
	
	SceneNode * root;
	Camera * camera;
};