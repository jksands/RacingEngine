#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include "Transform.h"

/// <summary>
/// A simple Camera implementation
/// </summary>
class Camera
{
private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;
	POINT prevMouse;
	float fovAngle;
	float nearClipDist;
	float farClipDist;
	float movementSpeed;
	float lookSpeed;
	// Entity* otherObject;
	bool following;

public:
	Camera(DirectX::XMFLOAT3 pos, float aspect, float moveSpeed, float lookSpeed);
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void LookAt(DirectX::XMFLOAT3 position);
	void Update(float dt, HWND windowHandle);
	Transform GetTransform();
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	void FollowObject(DirectX::XMFLOAT3 offset);
};

