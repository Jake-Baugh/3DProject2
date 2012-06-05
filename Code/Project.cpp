#include <Project.hpp>
#include <Camera\CircleCameraController.hpp>
#include <Camera\FreeCameraController.hpp>

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
	, mCamera(ProjectionDescription(mWindow.GetClientWidth(), mWindow.GetClientHeight()).Frustum.CreatePerspectiveProjection(), D3DXVECTOR3(0.0f, 30.0f, -20.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f))
	, mCameraController(new Camera::FreeCameraController(&mCamera))
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

	DirectionalLight dl;
	dl.Direction = D3DXVECTOR4(0.0, -1.0, 0.0, 0.0f);
	dl.Intensity = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.0f);

	mDeferredRenderer.SetDirectionalLight(dl);
	mDeferredRenderer.SetAmbientLight(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	
	/*
	PointLight p1;
	p1.Position =  D3DXVECTOR4(0.0f, 10.0f, 0.0f, 1.0f);
	p1.Intensity = D3DXVECTOR3(0.0f, 0.5f, 0.0f);
	p1.Radius = 20.0f;

	mDeferredRenderer.AddPointLight(p1);

	PointLight p2;
	p2.Position =  D3DXVECTOR4(10.0f, 10.0f, 10.0f, 1.0f);
	p2.Intensity = D3DXVECTOR3(0.5f, 0.0f, 0.5f);
	p2.Radius = 20.0f;

	mDeferredRenderer.AddPointLight(p2);
	*/
	
	
	float radius = 20;
	D3DXVECTOR3 intensities[] = { D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 1.0f) };
	int pointLightCount = sizeof(intensities) / sizeof(intensities[0]);
	for (int i = 0; i < pointLightCount; ++i)
	{
		double phi = 2.0 * D3DX_PI / pointLightCount;
		
		PointLight light;
		light.Position = D3DXVECTOR4(radius * cos(i * phi), 2.0f, radius * sin(i * phi), 1.0f);
		light.Intensity = intensities[i];
		light.Radius = 20.0f;

		mDeferredRenderer.AddPointLight(light);
	}
	
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
	mCameraController->Update(dt, mWindow.GetCurrentInput(), mWindow.GetPreviousInput());
	mCamera.Commit();
}

void Project::Draw(float dt)
{
	// Deferred stage
	mDeferredRenderer.BeginDeferredState();
	mGround.Draw(mCamera);

	mModel.Bind();
	mModel.Draw(D3DXVECTOR3(0.0f, 1.0f, 0.0f), mCamera);
	mDeferredRenderer.EndDeferredState();
	mDeferredRenderer.ApplyLightingPhase(mCamera);


	// Render G buffer
	mEffect.SetVariable("gTexture", mDeferredRenderer.GetFinalComposition());
	//mEffect.SetVariable("gTexture", mDeferredRenderer.mPositionSRV);

	mVertexBuffer.Bind();
	for (unsigned int p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
	{
		mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mD3DContext.GetDevice());
		mD3DContext.GetDevice()->Draw(mVertexBuffer.GetElementCount(), 0);
	}

	// Unbind G buffer
	//mEffect.SetVariable("gTexture", NULL);
	//mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).Apply(mD3DContext.GetDevice());
}
