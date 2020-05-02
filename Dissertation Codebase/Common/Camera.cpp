#include "Camera.h"
#include "Window.h"
#include <algorithm>

using namespace NCL;

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float dt) {
	

	float frameSpeed = keySensitivity * dt;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		position.z -= frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		position.z += frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		position.x -= frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		position.x += frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
		position.y += frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
		position.y -= frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
	{
		yaw -= frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
	{
		yaw += frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP))
	{
		pitch += frameSpeed;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN))
	{
		pitch -= frameSpeed;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM1))
	{
		keySensitivity = 200;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM2))
	{
		keySensitivity = 100;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM3))
	{
		keySensitivity = 50;
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM4))
	{
		keySensitivity = 25;
	}
}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix() const 
{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	/*return	Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-position);*/

	return Matrix4::Translation(-position)* Matrix4::Rotation(-yaw, Vector3(0, 1, 0))* Matrix4::Rotation(-pitch, Vector3(1, 0, 0));
};

Matrix4 Camera::BuildProjectionMatrix(float currentAspect) const {
	if (camType == CameraType::Orthographic) {
		return Matrix4::Orthographic(nearPlane, farPlane, right, left, top, bottom);
	}
	//else if (camType == CameraType::Perspective) {
		return Matrix4::Perspective(nearPlane, farPlane, currentAspect, fov);
	//}
}

Camera Camera::BuildPerspectiveCamera(const Vector3& pos, float pitch, float yaw, float fov, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Perspective;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane  = farPlane;

	c.fov		= fov;

	return c;
}
Camera Camera::BuildOrthoCamera(const Vector3& pos, float pitch, float yaw, float left, float right, float top, float bottom, float nearPlane, float farPlane) {
	Camera c;
	c.camType	= CameraType::Orthographic;
	c.position	= pos;
	c.pitch		= pitch;
	c.yaw		= yaw;
	c.nearPlane = nearPlane;
	c.farPlane	= farPlane;

	c.left		= left;
	c.right		= right;
	c.top		= top;
	c.bottom	= bottom;

	return c;
}