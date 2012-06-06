#include <Camera\SplineCameraController.hpp>

namespace Camera
{
	const float SplineCameraController::C_SPEED = 0.5f;

	SplineCameraController::SplineCameraController(Camera* camera, BezierCurve* spline, Targeting targetBehaviour, D3DXVECTOR3 target)
		: CameraController(camera)
		, mSpline(spline)
		, mBehaviour(targetBehaviour)
		, mTarget(target)
		, mDirection(mTarget - mCamera->GetPosition())
		, mProgress(0.0f)
	{
		D3DXVec3Normalize(&mDirection, &mDirection);
	}

	void SplineCameraController::Update(float dt, const Framework::InputState& currentInput, const Framework::InputState& previousInput)
	{
		// DEBUG
		if(currentInput.Keyboard.Keys['R'] && ! previousInput.Keyboard.Keys['R'])
			mProgress = 0;

		mProgress += dt * C_SPEED;

		if(mProgress >= mSpline->GetLength())
			return;

		switch(mBehaviour)
		{
			case Targeting::TargetPoint:
				mDirection = mTarget - mCamera->GetPosition();
				break;
			case Targeting::Spline:
				mDirection = mSpline->GetPos(mProgress + 0.01f) - mSpline->GetPos(mProgress);
				break;
			/*case Targeting::Unchanged:
				break;*/
		}

		D3DXVec3Normalize(&mDirection, &mDirection);
		mCamera->SetDirection(mDirection);
		mCamera->SetPosition(mSpline->GetPos(mProgress));
	}
}