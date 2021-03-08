#include "Camera.h"

using namespace DirectX;

Camera::Camera(DirectX::XMFLOAT3 pos, float aspect, float moveSpeed, float lookSpeed)
{
	transform = Transform(pos);
	this->movementSpeed = moveSpeed;
	this->lookSpeed = lookSpeed;

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspect);

}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, .01f, 1000));
}

// May want an overload that takes in a specific forward to implement basic "LookAt"
void Camera::UpdateViewMatrix()
{
	XMVECTOR pos = XMLoadFloat3(&(transform.GetPosition()));
	XMVECTOR quat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&(transform.GetPitchYawRoll())));

	// Define the standard forward vector
	XMVECTOR baseFor = XMVectorSet(0, 0, 1, 0);
	// Our direction
	XMVECTOR dir = XMVector3Rotate(baseFor, quat);
	XMMATRIX view = XMMatrixLookToLH(pos, dir, XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&viewMatrix, view);
}

// Will likely move controls to a GameLoop Update
void Camera::Update(float dt, HWND windowHandle)
{
	if (GetAsyncKeyState('W') & 0x8000) { transform.MoveRelative(0, 0, movementSpeed * dt); }
	if (GetAsyncKeyState('S') & 0x8000) { transform.MoveRelative(0, 0, -movementSpeed * dt); }
	if (GetAsyncKeyState('A') & 0x8000) { transform.MoveRelative(-movementSpeed * dt, 0, 0); }
	if (GetAsyncKeyState('D') & 0x8000) { transform.MoveRelative(movementSpeed * dt, 0, 0); }
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) { transform.MoveAbsolute(0, movementSpeed * dt, 0); }
	if (GetAsyncKeyState('X') & 0x8000) { transform.MoveAbsolute(0, -movementSpeed * dt, 0); }

	POINT mousePos = {};
	GetCursorPos(&mousePos);
	ScreenToClient(windowHandle, &mousePos);

	// IF left mouse is pressed
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		float deltaX = (mousePos.x - prevMouse.x) * dt * lookSpeed;
		float deltaY = (mousePos.y - prevMouse.y) * dt * lookSpeed;

		transform.Rotate(deltaY, deltaX, 0);

	}
	prevMouse = mousePos;

	UpdateViewMatrix();
}

Transform Camera::GetTransform()
{
	return transform;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projMatrix;
}
