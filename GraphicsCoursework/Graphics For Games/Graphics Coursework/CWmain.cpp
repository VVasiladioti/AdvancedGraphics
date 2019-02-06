#pragma comment(lib, "nclgl.lib")

#include "../../nclgl/window.h"
#include "Renderer.h"
#include "RendererScene2.h"
#include "RendererScene3.h"



int main() {
	Window w("Scene1", 1440, 1024, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());

		renderer.RenderScene();
	}

	return 0;
}




//T15+t10

/*
int main() {
	Window w("Scene2", 1440, 1024, false); 
	if (!w.HasInitialised()) {
		return -1;
	}

	srand((unsigned int)w.GetTimer()->GetMS() * 1000.0f);

	RendererScene2 renderer(w); 
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}
*/

//T7


/*
int main() {
	Window w("Deferred Rendering!", 1440, 1024, false); //This is all boring win32 window creation stuff!
	if (!w.HasInitialised()) {
		return -1;
	}

	srand((unsigned int)w.GetTimer()->GetMS() * 1000.0f);

	RendererScene3 renderer(w); //This handles all the boring OGL 3.2 initialisation stuff, and sets up our tutorial!
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}

*/
/*

int main() {
	Window w("Cube Mapping! sky textures courtesy of http://www.hazelwhorley.com", 1440, 1024, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	RendererScene2 renderer2(w);
	if (!renderer2.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	bool renderSwitch = true;

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {

			renderSwitch = true;
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {

			renderSwitch = false;

		}

		if (renderSwitch) {

			renderer.UpdateScene(w.GetTimer()->GetTimedMS());

			renderer.RenderScene();
		}

		else {
			renderer2.UpdateScene(w.GetTimer()->GetTimedMS());

			renderer2.RenderScene();

		}
	}

	return 0;
}

*/


/*
if (!wglMakeCurrent(deviceContext, tempContext)) {	// Try To Activate The Rendering Context
	std::cout << "OGLRenderer::OGLRenderer(): Cannot set temporary context!" << std::endl;
	wglDeleteContext(tempContext);
	return;
}

*/

