#ifndef CIRCLE_CAMERA_CONTROLLER_HPP
#define CIRCLE_CAMERA_CONTROLLER_HPP

#include <Camera\Camera.hpp>
#include <Camera\CameraController.hpp>

namespace Camera
{
	class CircleCameraController : public CameraController
	{
	public:
		CircleCameraController(Camera* camera, const D3DXVECTOR3& center, float radius, float height);
		~CircleCameraController() throw();

		void Update(float dt, const Framework::InputState& currentInput, const Framework::InputState& previousInput);

		static const float C_ROTATION_SPEED;
	private:
		D3DXVECTOR3 mCenter;
		float mRadius;
		float mHeight;
	
		float mPhi;
	};
}

#endif