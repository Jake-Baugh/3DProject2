#include <Project.hpp>
#include <CircleCameraController.hpp>

Project::WindowDescription::WindowDescription()
{
	Description.Caption = "3D Project 2";
	Description.ClientWidth = 1024;
	Description.ClientHeight = 768;
	Description.Resizable = false;
}

Project::ContextDescription::ContextDescription()
{
	Description.Fullscreen = false;
	Description.Viewports.push_back(Framework::D3DContext::Viewport());
}

Project::ProjectionDescription::ProjectionDescription(unsigned int clientWidth, unsigned int clientHeight)
{
	Frustum.NearDistance = 1.0f;
	Frustum.FarDistance = 1000.0f;
	Frustum.FieldOfViewY = D3DX_PI * 0.25;
	Frustum.AspectRatio = static_cast<float>(clientWidth) / static_cast<float>(clientHeight);
}

Project::Project(HINSTANCE instance)
	: Game(instance, WindowDescription().Description, ContextDescription().Description)
	, mCamera(ProjectionDescription(mWindow.GetClientWidth(), mWindow.GetClientHeight()).Frustum.CreatePerspectiveProjection(), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f))
	, mCameraController(new CircleCameraController(&mCamera, D3DXVECTOR3(0, 0, 0), 15.0f, 2.0f))
	, mDeferredRenderer(&mD3DContext, 1024, 768)
	, mModel(mD3DContext.GetDevice(), "Pacman2.obj")
	, mGround(mD3DContext.GetDevice())

	, mVertexBuffer(mD3DContext.GetDevice())
	, mEffect(mD3DContext.GetDevice(), "Resources/Effects/Quad2D.fx")
{
	/*
	Project::QuadVertex vertices[] = { { D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f) }
									 , { D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f) }
									 , { D3DXVECTOR2(0.0, 0.0f), D3DXVECTOR2(0.0f, 1.0f) }
									 , { D3DXVECTOR2(1.0, 0.0f), D3DXVECTOR2(1.0f, 1.0f) } };
	*/
	Project::QuadVertex vertices[] = { { D3DXVECTOR2(-1.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f) }
									 , { D3DXVECTOR2(1.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f) }
									 , { D3DXVECTOR2(-1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f) }
									 , { D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f) } };

	Framework::VertexBuffer::Description bufferDesc;
	bufferDesc.ElementCount = 4;
	bufferDesc.ElementSize = sizeof(Project::QuadVertex);
	bufferDesc.Topology = Framework::Topology::TriangleStrip;
	bufferDesc.Usage = Framework::Usage::Default;
	bufferDesc.FirstElementPointer = vertices;

	mVertexBuffer.SetData(bufferDesc, NULL);


	Framework::Effect::InputLayoutVector inputLayout;
	inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32_FLOAT));
	inputLayout.push_back(Framework::Effect::InputLayoutElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT));

	mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);
}

Project::~Project() throw()
{
	SafeDelete(mCameraController);
}

void Project::KeyPressed(Framework::ApplicationWindow* window, int keyCode)
{
	switch (keyCode)
	{
		case VK_ESCAPE:
			Quit();
			break;
	}
}

void Project::Update(float dt)
{
	mCameraController->Update(dt);
	mCamera.Commit();
}

void Project::Draw(float dt)
{
	// Deferred stage
	mDeferredRenderer.BeginDeferredState();

	mGround.Draw(mCamera);

	mDeferredRenderer.EndDeferredState();


	// Render G buffer
	/*
	D3DXMATRIX mvp;
	D3DXMatrixIdentity(&mvp);

	mEffect.SetVariable("gMVP", mvp);
	mEffect.SetVariable("gTexture", mDeferredRenderer.mDepthStencilSRV);
	mEffect.SetVariable("gNear", 1.0f);
	mEffect.SetVariable("gFar", 1000.0f);

	mVertexBuffer.Bind();
	for (unsigned int p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
	{
		mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mD3DContext.GetDevice());
		mD3DContext.GetDevice()->Draw(mVertexBuffer.GetElementCount(), 0);
	}
	*/

	// Unbind G buffer
	//mEffect.SetVariable("gTexture", NULL);
	//mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).Apply(mD3DContext.GetDevice());
}
