#include "Camera.h"
#include "Helpers.h"
#include "Entity.h"
#include "EntityManager.h"

using namespace DirectX;
Entity* otherObject;
XMFLOAT3 offset;
SHORT prevQ = 0;

Camera::Camera(DirectX::XMFLOAT3 pos, float aspect, float moveSpeed, float lookSpeed)
{
	transform = Transform(pos);
	this->movementSpeed = moveSpeed;
	this->lookSpeed = lookSpeed;

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspect);

	following = false;
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

void Camera::LookAt(DirectX::XMFLOAT3 position)
{
	XMVECTOR pos = XMLoadFloat3(&(transform.GetPosition())); 
	position.y += 2;
	XMVECTOR otherPos = XMLoadFloat3(&position);
	// XMVECTOR quat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&(transform.GetPitchYawRoll())));

	// Define the standard forward vector
	// XMVECTOR baseFor = XMVectorSet(0, 0, 1, 0);
	
	// Our direction
	// XMVECTOR dir = XMVector3Rotate(baseFor, quat);
	XMMATRIX view = XMMatrixLookToLH(pos, otherPos - pos, XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&viewMatrix, view);
}

// Will likely move controls to a GameLoop Update
void Camera::Update(float dt, HWND windowHandle)
{
	float moveSpeed = 1.0f;
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000) moveSpeed = 4.0f;
	if (GetAsyncKeyState(VK_UP) & 0x8000) { transform.MoveRelative(0, 0, movementSpeed * dt * moveSpeed); }
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) { transform.MoveRelative(0, 0, -movementSpeed * dt * moveSpeed); }
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) { transform.MoveRelative(-movementSpeed * dt * moveSpeed, 0, 0); }
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { transform.MoveRelative(movementSpeed * dt * moveSpeed, 0, 0); }
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) { transform.MoveAbsolute(0, movementSpeed * dt * moveSpeed, 0); }
	if (GetAsyncKeyState('X') & 0x8000) { transform.MoveAbsolute(0, -movementSpeed * dt * moveSpeed, 0); }
	if (!prevQ && GetAsyncKeyState('Q') & 0x8000) { following = !following; }
	prevQ = GetAsyncKeyState('Q');
	POINT mousePos = {};
	GetCursorPos(&mousePos);
	ScreenToClient(windowHandle, &mousePos);

	// IF left mouse is pressed
	if (GetAsyncKeyState(VK_LBUTTON) && !following)
	{
		float deltaX = (mousePos.x - prevMouse.x) * dt * lookSpeed;
		float deltaY = (mousePos.y - prevMouse.y) * dt * lookSpeed;

		transform.Rotate(deltaY, deltaX, 0);

	}
	if (following)
	{
		// Position camera based on otherObject's transform
		XMFLOAT3 other = otherObject->GetTransform()->GetPosition();
		// Get other object's rotation
		XMVECTOR quat = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&(otherObject->GetTransform()->GetPitchYawRoll())));
		XMVECTOR tempOffset = XMLoadFloat3(&offset);
		// Rotate offset to match car's rotation
		tempOffset = XMVector3Rotate(tempOffset, quat);
		XMFLOAT3 temp;
		// Store result back into offset
		XMStoreFloat3(&temp, tempOffset); // Can't store back into original since it'll add ALL rotations
		XMFLOAT3 sum = AddFloat3(other, temp);
		transform.SetPosition(sum.x, sum.y, sum.z);
		// Orient camera based on otherObject's transform
		LookAt(other);
		other = otherObject->GetTransform()->GetPitchYawRoll();
		transform.SetRotation(other.x, other.y, other.z);
	}
	prevMouse = mousePos;

	if (!following)
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

void Camera::FollowObject(XMFLOAT3 _offset)
{
	// Can't make otherObject a parameter since it will introduce a circular dependency
	// Instead do it this janky way
	otherObject = EntityManager::GetInstance()->GetEntities()[0];
	offset = XMFLOAT3(0,5,10);
}
