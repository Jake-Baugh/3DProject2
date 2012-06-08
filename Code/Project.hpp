#ifndef PROJECT_HPP
#define PROJECT_HPP

#include <vector>
#include <Framework\Game.hpp>
#include <Framework\VertexBuffer.hpp>
#include <Framework\Effect\Effect.hpp>
#include <Helper\BezierCurve.hpp>
#include <Helper\DrawableFrustum.hpp>
#include <Resources\ModelObj.hpp>
#include <Camera\Camera.hpp>
#include <Camera\CameraController.hpp>
#include <DeferredRenderer.hpp>
#include <Helper\MorphAnimation.hpp>
#include <Scene\Scene.hpp>

// DEBUG
#include <Helper\DrawableSpline.hpp>
#include <Camera\SplineCameraController.hpp>

class Project : public Framework::Game
{
public:
	Project(HINSTANCE instance);
	~Project() throw();

	void KeyPressed(Framework::ApplicationWindow* window, int keyCode);
protected:
	void Update(float dt);
	void Draw(float dt);
private:
	struct WindowDescription
	{
		Framework::ApplicationWindow::Description Description;

		WindowDescription();
	};

	struct ContextDescription
	{
		Framework::D3DContext::Description Description;

		ContextDescription();
	};

	struct ProjectionDescription
	{
		Helper::Frustum Frustum;

		ProjectionDescription(unsigned int clientWidth, unsigned int clientHeight);
	};

	ProjectionDescription mProjectionDescription;
	BezierCurve mCameraSpline;
	Camera::Camera mCamera;
	Camera::CameraController* mCameraController;

	DeferredRenderer mDeferredRenderer;
	int mBufferToRender;

	Scene::Scene mScene;
	Helper::DrawableFrustum mDrawableFrustum;
	Helper::MorphAnimation* mAnimation;

	// DEBUG
	Helper::DrawableSpline mCameraCurve;
};

#endif