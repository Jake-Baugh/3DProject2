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
	, mBufferToRender(-1)
{
	DirectionalLight dl;
	dl.Direction = D3DXVECTOR4(0.0, -1.0, 0.0, 0.0f);
	dl.Intensity = D3DXVECTOR4(0.5f, 0.0f, 0.5f, 0.0f);

	
	mDeferredRenderer.SetDirectionalLight(dl);
	mDeferredRenderer.SetAmbientLight(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	
	float radius = 20;
	D3DXVECTOR3 intensities[] = { D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 1.0f) };
	int pointLightCount = sizeof(intensities) / sizeof(intensities[0]);
	for (int i = 0; i < pointLightCount; ++i)
	{
		double phi = 2.0 * D3DX_PI / pointLightCount;
		
		PointLight light;
		light.Position = D3DXVECTOR4(radius * cos(i * phi), 1.0f, radius * sin(i * phi), 1.0f);
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

		case VK_F1:
			mBufferToRender = -1;	// Final composition
		break;

		case VK_F2:
			mBufferToRender = DeferredRenderer::C_GBUFFER_COLOR;
		break;

		case VK_F3:
			mBufferToRender = DeferredRenderer::C_GBUFFER_POSITION;
		break;

		case VK_F4:
			mBufferToRender = DeferredRenderer::C_GBUFFER_NORMAL;
		break;

		case VK_F5:
			mBufferToRender = DeferredRenderer::C_GBUFFER_MATERIAL;
		break;

		case VK_F6:
			mBufferToRender = DeferredRenderer::C_GBUFFER_DEPTH;
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

	if (mBufferToRender == -1)
		mDeferredRenderer.RenderFinalComposition();
	else
		mDeferredRenderer.RenderBuffer(mDeferredRenderer.GetGBufferByIndex(mBufferToRender));
}
