#ifndef CAMERA_CONTROLLER_HPP
#define CAMERA_CONTROLLER_HPP

#include <Helper\Camera.hpp>

class CameraController
{
public:
	CameraController(Helper::Camera* camera);
	virtual ~CameraController() throw();

	virtual void Update(float dt) = 0;
protected:
	Helper::Camera* mCamera;
};

#endif