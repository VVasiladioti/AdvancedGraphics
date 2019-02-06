#include "Renderer.h"
#include "../../nclgl/GameTimer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {
	camera = new Camera();
	heightMap = new HeightMap(TEXTUREDIR "terrain.raw");
	quad = Mesh::GenerateQuad();
	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500.0f, RAW_WIDTH * HEIGHTMAP_X));
	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f), 1500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)), Vector4 (1.0f, 0.5f, 0.31f, 1), (RAW_WIDTH * HEIGHTMAP_X) / 0.5f);
		//Vector4(0.9f, 0.9f, 1.0f, 1) colour
		
	reflectShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR "skyboxVertex.glsl", SHADERDIR "skyboxFragment.glsl");
	lightShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "PerPixelFragment.glsl");

	hellData = new MD5FileData(MESHDIR "hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);

	hellData->AddAnim(MESHDIR "idle2.md5anim");
	hellNode->PlayAnim(MESHDIR "idle2.md5anim");

	sceneShader = new Shader(SHADERDIR "shadowscenevert.glsl", SHADERDIR "shadowscenefrag.glsl");
	shadowShader = new Shader(SHADERDIR "shadowVert.glsl", SHADERDIR "shadowFrag.glsl");
	
	if (!sceneShader->LinkProgram() || !shadowShader->LinkProgram()) {
		return;
	}

	if (!reflectShader->LinkProgram() || !lightShader->LinkProgram() ||
		!skyboxShader->LinkProgram()) {
		return;
	}

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Canyon Rock.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)); 
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "Canyon RockDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)); 
	
	//Skybox courtesy of 'The Mighty Pete'"
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR "habitual-pain_lf.tga", TEXTUREDIR "habitual-pain_rt.tga", TEXTUREDIR "habitual-pain_up.tga",
		TEXTUREDIR "habitual-pain_dn.tga", TEXTUREDIR "habitual-pain_ft.tga", TEXTUREDIR "habitual-pain_bk.tga", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap || !quad->GetTexture() || !heightMap->GetTexture() ||
		!heightMap->GetBumpMap()) {
		return;

	}

	SetTextureRepeating(quad->GetTexture(), true);
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	init = true;
	waterRotate = 0.0f;

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader;
	delete light;

	delete hellData;
	delete hellNode;
	delete sceneShader;
	delete shadowShader;
	currentShader = 0;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Z)) {
		hellData->AddAnim(MESHDIR "walk7.md5anim");
		hellNode->PlayAnim(MESHDIR "walk7.md5anim");
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_X)) {
		hellData->AddAnim(MESHDIR "idle2.md5anim");
		hellNode->PlayAnim(MESHDIR "idle2.md5anim");
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_U)) {
		float PositionX;
		PositionX = light->GetPosition().x;
		light->SetPosition(Vector3(PositionX + 10, 1500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)));
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_J)) {
		float PositionX;
		PositionX = light->GetPosition().x;
		light->SetPosition(Vector3(PositionX - 10, 1500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)));
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_I)) {
		float PositionY;
		PositionY = light->GetPosition().y;
		light->SetPosition(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f), PositionY + 10, (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)));
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) {
		float PositionY;
		PositionY = light->GetPosition().y;
		light->SetPosition(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f),PositionY - 10, (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)));
	}
	
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_O)) {
		float PositionZ;
		PositionZ = light->GetPosition().z;
		light->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 1.0f, 1500.0f, (PositionZ + 10)));
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) {
		float PositionZ;
		PositionZ = light->GetPosition().z;
		light->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 1.0f, 1500.0f, (PositionZ - 10)));
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_N)) {
		light->SetPosition(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 1.0f), 1500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 1.0f)));
	}

	waterRotate += msec / 1000.0f;
	hellNode->Update(msec); 
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();
	DrawHeightmap();
	DrawWater();
	DrawShadowScene(); // First render pass ...
	DrawCombinedScene(); // Second render pass ...

	SwapBuffers();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	UpdateShaderMatrices();
	quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap() {
	SetCurrentShader(lightShader);

	SetCurrentShader(sceneShader);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),"cameraPos"), 1, (float *)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"shadowTex"), 8);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),"cameraPos"), 1, (float *)& camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);
	float heightY = 256 * HEIGHTMAP_Y / 3.0f;
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix = Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
	UpdateShaderMatrices();
	quad->Draw();
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	SetCurrentShader(shadowShader);

	projMatrix = Matrix4::Perspective(500.0f, 15000.0f, (float)width / (float)height, 45.0f);
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	shadowMatrix = biasMatrix * (projMatrix * viewMatrix);

	UpdateShaderMatrices();
	DrawHeightmap();
	DrawMesh();

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"bumpTex"), 1);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),"cameraPos"), 1, (float *)& camera->GetPosition());
	SetShaderLight(*light);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"shadowTex"), 8);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	DrawMesh();
}

void Renderer::DrawMesh() {
	modelMatrix.ToIdentity();

	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);

	modelMatrix = Matrix4::Translation(Vector3((RAW_HEIGHT*HEIGHTMAP_X / (2.0f - 0.5f)), 250.0f, (RAW_HEIGHT*HEIGHTMAP_Z / 2.0f)))
		*Matrix4::Scale(Vector3(1, 1, 1))*
		Matrix4::Rotation(90, Vector3(0, 1, 0));

	UpdateShaderMatrices();
	hellNode->Draw(*this);
}
