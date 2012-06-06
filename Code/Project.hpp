#ifndef PROJECT_HPP
#define PROJECT_HPP

#include <Framework\Game.hpp>
#include <Framework\VertexBuffer.hpp>
#include <Framework\Effect\Effect.hpp>
#include <Helper\BezierCurve.hpp>
#include <Resources\ModelObj.hpp>
#include <Camera\Camera.hpp>
#include <Camera\CameraController.hpp>
#include <Ground.hpp>
#include <DeferredRenderer.hpp>

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

	Camera::Camera mCamera;
	Camera::CameraController* mCameraController;

	DeferredRenderer mDeferredRenderer;
	int mBufferToRender;

	Resources::ModelObj mModel;
	Ground mGround;
	BezierCurve mCurve;
	Framework::VertexBuffer mCurveBuffer;
	Framework::Effect::Effect mCurveEffect;
};

#endif