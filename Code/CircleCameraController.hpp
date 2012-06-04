#ifndef CIRCLE_CAMERA_CONTROLLER_HPP
#define CIRCLE_CAMERA_CONTROLLER_HPP

#include <Helper\Camera.hpp>
#include <CameraController.hpp>

class CircleCameraController : public CameraController
{
public:
	CircleCameraController(Helper::Camera* camera, const D3DXVECTOR3& center, float radius, float height);
	~CircleCameraController() throw();

	void Update(float dt);

	static const float C_ROTATION_SPEED;
private:
	D3DXVECTOR3 mCenter;
	float mRadius;
	float mHeight;
	
	float mPhi;
};

#endif