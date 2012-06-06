#ifndef SPLINE_CAMERA_CONTROLLER_HPP
#define SPLINE_CAMERA_CONTROLLER_HPP

#include <Helper\Global.hpp>
#include <Camera\CameraController.hpp>
#include <Helper\BezierCurve.hpp>

namespace Camera
{
	class SplineCameraController : public CameraController
	{
	public:
		enum Targeting
		{
			TargetPoint,
			Unchanged,
			Spline
		};

		SplineCameraController(Camera* camera, BezierCurve* spline, Targeting targetBehaviour, D3DXVECTOR3 target = D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		void Update(float dt, const Framework::InputState& currentInput, const Framework::InputState& previousInput);

	private:
		static const float C_SPEED;

		BezierCurve* mSpline;
		Targeting mBehaviour;

		D3DXVECTOR3 mTarget;		// Used for behaviour TargetPoint and when calculating the Unchanged behaviour direction
		D3DXVECTOR3 mDirection;		// Used all the time, will be updated once for Unchanged and every update for other behaviours
		float mProgress;
	};
}
#endif