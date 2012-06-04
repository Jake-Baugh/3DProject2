#include <cmath>
#include "CircleCameraController.hpp"

const float CircleCameraController::C_ROTATION_SPEED = D3DX_PI * .6f;

CircleCameraController::CircleCameraController(Helper::Camera* camera, const D3DXVECTOR3& center, float radius, float height)
	: CameraController(camera)
	, mCenter(center)
	, mRadius(radius)
	, mHeight(height)
	, mPhi(0)
{}

CircleCameraController::~CircleCameraController() throw()
{}

void CircleCameraController::Update(float dt)
{
	mPhi += C_ROTATION_SPEED * dt;

	mCamera->SetPosition(mCenter + D3DXVECTOR3(mRadius * std::cos(mPhi), mHeight, mRadius * std::sin(mPhi)));
	mCamera->SetFacingPoint(mCenter);
}