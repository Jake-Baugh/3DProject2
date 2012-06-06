#include <Camera\CameraController.hpp>

namespace Camera
{
	CameraController::CameraController(Camera* camera)
		: mCamera(camera)
	{}

	CameraController::~CameraController() throw()
	{}
}