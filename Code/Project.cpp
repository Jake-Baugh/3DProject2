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
	, mModel(mD3DContext.GetDevice(), "Pacman2.obj")
	, mCameraController(new CircleCameraController(&mCamera, D3DXVECTOR3(0, 0, 0), 15.0f, 10.0f))
	, mGround(mD3DContext.GetDevice())
{}

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
	mGround.Draw(mCamera);
	mModel.Bind();
	mModel.Draw(D3DXVECTOR3(0.0f, 1.0f, 0.0f), mCamera);
}
