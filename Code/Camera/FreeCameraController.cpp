#include <Camera\FreeCameraController.hpp>
#include <Helper\Global.hpp>

namespace Camera
{
	const double FreeCameraController::C_MAX_PITCH = D3DX_PI * 0.49;
	const float FreeCameraController::C_MOVE_SPEED = 50.0f;
	const float FreeCameraController::C_ROTATE_SPEED = D3DX_PI;

	FreeCameraController::FreeCameraController(Camera* camera)
		: CameraController(camera)
	{}

	void FreeCameraController::Update(float dt, const Framework::InputState& currentInput, const Framework::InputState& previousInput)
	{
		// Collect input
		int moveForward = 0;
		int moveRight = 0;
		int rotateLeft = 0;
		int rotateUp = 0;
		
		if (currentInput.Keyboard.Keys['W'] || currentInput.Keyboard.Keys[VK_UP])
			moveForward = 1;
		if (currentInput.Keyboard.Keys['S'] || currentInput.Keyboard.Keys[VK_DOWN])
			moveForward = -1;
		if (currentInput.Keyboard.Keys['A'] || currentInput.Keyboard.Keys[VK_LEFT])
			moveRight = -1;
		if (currentInput.Keyboard.Keys['D'] || currentInput.Keyboard.Keys[VK_RIGHT])
			moveRight = 1;

		if (currentInput.Keyboard.Keys[VK_NUMPAD9])
			moveRight = 1;
		if (currentInput.Keyboard.Keys[VK_NUMPAD7])
			moveRight = -1;
		
		if (currentInput.Keyboard.Keys[VK_NUMPAD8])
			rotateUp = 1;
		if (currentInput.Keyboard.Keys[VK_NUMPAD2])
			rotateUp = -1;
		if (currentInput.Keyboard.Keys[VK_NUMPAD4])
			rotateLeft = 1;
		if (currentInput.Keyboard.Keys[VK_NUMPAD6])
			rotateLeft = -1;


		// Get the polar angles of the current camera direction and rotate it
		D3DXVECTOR3 direction = mCamera->GetDirection();
		float radius = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
		float yaw = std::atan2(direction.z, direction.x);
		float pitch = std::asin(direction.y / radius);

		yaw += C_ROTATE_SPEED * rotateLeft * dt;
		pitch += C_ROTATE_SPEED * rotateUp * dt;
		
		if (pitch <= -C_MAX_PITCH)
			pitch = -C_MAX_PITCH;
		if (pitch >= C_MAX_PITCH)
			pitch = C_MAX_PITCH;

		float aux = radius * std::cos(pitch);
		direction.x = aux * std::cos(yaw);
		direction.y = radius * std::sin(pitch);
		direction.z = aux * std::sin(yaw);


		// Move the camera
		D3DXVECTOR3 position = mCamera->GetPosition();
		D3DXVECTOR3 right;
		D3DXVec3Cross(&right, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &direction);
		D3DXVec3Normalize(&right, &right);

		position += direction * moveForward * C_MOVE_SPEED * dt;
		position += right * moveRight * C_MOVE_SPEED * dt;


		// Set the new direction and position
		mCamera->SetPosition(position);
		mCamera->SetDirection(direction);
	}
}