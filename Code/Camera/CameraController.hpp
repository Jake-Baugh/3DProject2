#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include <Framework\ApplicationWindow.hpp>
#include <Camera\Camera.hpp>

namespace Camera
{
	class CameraController
	{
	public:
		CameraController(Camera* camera);
		virtual ~CameraController() throw();

		virtual void Update(float dt, const Framework::InputState& currentInput, const Framework::InputState& previousInput) = 0;
	protected:
		Camera* mCamera;
	};
}

#endif