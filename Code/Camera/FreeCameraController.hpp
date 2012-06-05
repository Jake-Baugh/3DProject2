#ifndef FREE_CAMERA_CONTROLLER_HPP
#define FREE_CAMERA_CONTROLLER_HPP

#include <Camera\CameraController.hpp>

namespace Camera
{
	class FreeCameraController : public CameraController
	{
	public:
		FreeCameraController(Camera* camera);

		void Update(float dt, const Framework::InputState& currentInput, const Framework::InputState& previousInput);

		static const double C_MAX_PITCH;
		static const float C_MOVE_SPEED;
		static const float C_ROTATE_SPEED;
	};
}

#endif