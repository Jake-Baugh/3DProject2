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
	, mUseDebugFrustum(false)
	, mDebugFrustumPosition(0.0f, 0.0f, 0.0f)
	, mDebugFrustumDirection(0.0f, 0.0f, 0.0f)
	, mProjectionDescription(mWindow.GetClientWidth(), mWindow.GetClientHeight())
	, mCameraSpline(D3DXVECTOR3(-20, 30, -20), D3DXVECTOR3(-20, 0, -20), D3DXVECTOR3(20, 60, 20), D3DXVECTOR3(20,30,20))
	, mCamera(mProjectionDescription.Frustum.CreatePerspectiveProjection(), D3DXVECTOR3(0.0f, 30.0f, -20.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f))
	//, mCameraController(new Camera::SplineCameraController(&mCamera, &mCameraSpline, Camera::SplineCameraController::Spline))
	, mCameraController(new Camera::FreeCameraController(&mCamera))
	, mDeferredRenderer(&mD3DContext, 1024, 768)
	, mBufferToRender(-1)
	, mScene(mD3DContext.GetDevice())
	, mDrawableFrustum(mD3DContext.GetDevice(), ProjectionDescription(mWindow.GetClientWidth(), mWindow.GetClientHeight()).Frustum, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f))
	, mAnimation(NULL)
	, mCameraCurve(mD3DContext.GetDevice(), &mCameraSpline) // DEBUG
{
	DirectionalLight dl;
	dl.Direction = D3DXVECTOR4(0.0, -1.0, 0.0, 0.0f);
	dl.Intensity = D3DXVECTOR4(0.5f, 0.0f, 0.5f, 0.0f);
	
	mDeferredRenderer.SetDirectionalLight(dl);
	mDeferredRenderer.SetAmbientLight(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	
	const float RADIUS = 20;
	D3DXVECTOR3 intensities[] = { D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 1.0f) };
	int pointLightCount = sizeof(intensities) / sizeof(intensities[0]);
	for (int i = 0; i < pointLightCount; ++i)
	{
		double phi = 2.0 * D3DX_PI / pointLightCount;
		
		PointLight light;
		light.Position = D3DXVECTOR4(RADIUS * cos(i * phi), 1.0f, RADIUS * sin(i * phi), 1.0f);
		light.Intensity = intensities[i];
		light.Radius = 20.0f;

		mDeferredRenderer.AddPointLight(light);
	}

	mDrawableFrustum.Update(mCamera.GetPosition(), mCamera.GetDirection());


	std::vector<std::string> keyFrameObjs;
	keyFrameObjs.push_back("Pacman1.obj");
	keyFrameObjs.push_back("Pacman2.obj");

	std::vector<float> keyFrameTimes;
	keyFrameTimes.push_back(0.25f);
	keyFrameTimes.push_back(1.0f);

	mAnimation = new Helper::MorphAnimation(mD3DContext.GetDevice(), keyFrameObjs, keyFrameTimes, "glow.png");
}

Project::~Project() throw()
{
	SafeDelete(mCameraController);
	SafeDelete(mAnimation);
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

		case 'F':
			mUseDebugFrustum = !mUseDebugFrustum;
			
			if (mUseDebugFrustum)
			{
				mDebugFrustumPosition = mCamera.GetPosition();
				mDebugFrustumDirection = mCamera.GetDirection();
				mDrawableFrustum.Update(mCamera.GetPosition(), mCamera.GetDirection());
			}
		break;
	}
}

void Project::Update(float dt)
{
	mAnimation->Update(dt);

	mCameraController->Update(dt, mWindow.GetCurrentInput(), mWindow.GetPreviousInput());
	mCamera.Commit();
}

void Project::Draw(float dt)
{
	// Deferred stage
	mDeferredRenderer.BeginDeferredState();

	if (mUseDebugFrustum)
	{
		mScene.Draw(mCamera, mProjectionDescription.Frustum, mDebugFrustumPosition, mDebugFrustumDirection);
	}
	else
	{
		mScene.Draw(mCamera, mProjectionDescription.Frustum, mCamera.GetPosition(), mCamera.GetDirection());
	}

	D3DXMATRIX world;
	//D3DXMatrixTranslation(&world, 0, 10, 0);
	D3DXMatrixIdentity(&world);

	mAnimation->Draw(mCamera, world);
	mAnimation->DrawAABB(mCamera, world);

	//mCameraCurve.Draw(mCamera);

	if (mUseDebugFrustum)
	{
		mDrawableFrustum.Draw(mCamera);
	}

	mDeferredRenderer.EndDeferredState();
	mDeferredRenderer.ApplyLightingPhase(mCamera);

	if (mBufferToRender == -1)
		mDeferredRenderer.RenderFinalComposition();
	else
		mDeferredRenderer.RenderBuffer(mDeferredRenderer.GetGBufferByIndex(mBufferToRender));
}
