#pragma once

#include "../../nclGL/OGLRenderer.h"
#include "../../nclGL/camera.h"
#include "../../nclGL/HeightMap.h"

class Rendererscene : public OGLRenderer {
public:
	Renderer(Window & parent);
	virtual ~Renderer(void);
	
	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	HeightMap* heightMap;
	Camera* camera;
};
