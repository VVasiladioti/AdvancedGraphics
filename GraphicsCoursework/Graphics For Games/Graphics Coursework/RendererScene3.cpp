#include "RendererScene3.h"

RendererScene3::RendererScene3(Window &parent) : OGLRenderer(parent) {
	CubeRobot::CreateCube();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	camera = new Camera();
	camera->SetPosition(Vector3(0, 100, 750.0f));

	robotShader = new Shader(SHADERDIR "SceneVertex.glsl",SHADERDIR "SceneFragment.glsl");
	geomShader = new Shader(SHADERDIR "vertex.glsl", SHADERDIR "fragment.glsl", SHADERDIR "geometry.glsl");

	floor = Mesh::GenerateQuad();
	floor->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "Lava.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	floor->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR "LavaDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	
	if (!robotShader->LinkProgram() || !floor->GetTexture()) {
		return;
	}

	if (!geomShader->LinkProgram()) {
		return;
	}
	
	emitter = new ParticleEmitter();
	root = new SceneNode();
	
	for (int i = 0; i < 5; ++i) {
		SceneNode * s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.f, 1.0f, 0.5f));
		s->SetTransform(Matrix4::Translation(
			Vector3(0, 100.0f, -300.0f + 100.0f + 100 * i)));
		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetBoundingRadius(100.0f);
		s->SetMesh(floor); 
		root->AddChild(s);
	}

	root->AddChild(new CubeRobot());

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init = true;
}

RendererScene3 ::~RendererScene3(void) {
	delete root;
	delete floor;
	delete emitter;
	delete camera;
	CubeRobot::DeleteCube();
}

void RendererScene3::UpdateScene(float msec) {
	emitter->Update(msec);
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	root->Update(msec);
}

void RendererScene3::BuildNodeLists(SceneNode * from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}

	for (vector < SceneNode * >::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void RendererScene3::SortNodeLists() {
	std::sort(transparentNodeList.begin(),transparentNodeList.end(),SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),nodeList.end(),SceneNode::CompareByCameraDistance);
}

void RendererScene3::DrawNodes() {
	for (vector < SceneNode * >::const_iterator i = nodeList.begin();
		i != nodeList.end(); ++i) {
		DrawNode((*i));
	}
}

void RendererScene3::DrawNode(SceneNode * n) {
	if (n->GetMesh()) {
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float *)&(n->GetWorldTransform()* Matrix4::Scale(n->GetModelScale())));
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(),"nodeColour"), 1, (float *)& n->GetColour());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(),"useTexture"), (int)n->GetMesh()->GetTexture());

		n->Draw(*this);
	}
}

void RendererScene3::RenderScene() {
	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	SetCurrentShader(robotShader);
	glUseProgram(robotShader->GetProgram());
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(robotShader->GetProgram(),"diffuseTex"), 0);
	
	DrawNodes();
	glUniform1i(glGetUniformLocation(robotShader->GetProgram(),"useTexture"), 1);
	DrawFloor();

	SetCurrentShader(geomShader);
	glUseProgram(geomShader->GetProgram());
	glUniform1i(glGetUniformLocation(geomShader->GetProgram(),"diffuseTex"), 0);

	SetShaderParticleSize(emitter->GetParticleSize());
	emitter->SetParticleSize(8.0f);
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(16.0f);
	emitter->SetParticleLifetime(2000.0f);
	emitter->SetParticleSpeed(0.001f);
	modelMatrix = Matrix4::Translation(Vector3(0, 30, 0)) * Matrix4::Scale(Vector3(100,100,100));
	UpdateShaderMatrices();

	emitter->Draw();
	
	glUseProgram(0);
	SwapBuffers();
	ClearNodeLists();
}

void RendererScene3::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(geomShader->GetProgram(), "particleSize"), f);
}

void RendererScene3::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void RendererScene3::DrawFloor() {
	modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) *
		Matrix4::Scale(Vector3(450, 450, 1));
	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(),"textureMatrix"), 1, false, *& tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *& modelMatrix.values);

	floor->Draw();
}
