#ifndef PROJECT_HPP
#define PROJECT_HPP

#include <Framework\Game.hpp>
#include <Helper\Camera.hpp>
#include <Resources\ModelObj.hpp>
#include <CameraController.hpp>
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

	Helper::Camera mCamera;
	CameraController* mCameraController;

	DeferredRenderer mDeferredRenderer;
	Resources::ModelObj mModel;
	Ground mGround;

	struct QuadVertex
	{
		D3DXVECTOR2 Position;
		D3DXVECTOR2 UV;
	};

	Framework::VertexBuffer mVertexBuffer;
	Framework::Effect::Effect mEffect;
};

#endif