#include "Transform.h"
using namespace DirectX;

#pragma region CONSTRUCTORS
/// <summary>
/// Creates a basic Transform.  All parameters have default values.
/// </summary>
/// <param name="position">Location in space</param>
/// <param name="scale">Size of object</param>
/// <param name="rotation">Orientation of object</param>
Transform::Transform(DirectX::XMFLOAT3 _position, DirectX::XMFLOAT3 _scale, DirectX::XMFLOAT3 _rotation)
{
	position = _position;
	scale = _scale;
	rotation = _rotation;
	// Start with Matrix being dirty in case user passed params in
	matrixDirty = true;
	// Construct the world matrix, ignore the return value since the method itself constructs it
	// It will also set matrixDirty to false
	GetWorldMatrix();
}

/// <summary>
/// Creates a new Transform with the exact position of the one passed in
/// Useful if you want to spawn a bunch of things at the same position
/// </summary>
/// <param name="t">Transform to copy</param>
Transform::Transform(Transform* t)
{
	// this shouldn't create a depencency since these fields aren't pointers
	position = t->GetPosition();
	scale = t->GetScale();
	rotation = t->GetPitchYawRoll();
	matrixDirty = false;
	worldMatrix = t->GetWorldMatrix();
}
#pragma endregion

#pragma region SETTERS
void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3{ x,y,z };
	matrixDirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation = XMFLOAT3{ pitch,yaw,roll };
	matrixDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3{ x,y,z };
	matrixDirty = true;
}

#pragma endregion

#pragma region GETTERS
DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	// Create the matrix when necessary
	// if is different... then change
	// Check matrix's stored values against current rotation, scale, position
	// Update, then return
	if (matrixDirty)
	{
		XMMATRIX tempPos = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX tempScale = XMMatrixScaling(scale.x, scale.y, scale.z);
		XMMATRIX tempRot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		XMMATRIX world = tempScale * tempRot * tempPos;
		XMStoreFloat4x4(&worldMatrix, world);
		matrixDirty = false;
	}
	return worldMatrix;
}

#pragma endregion

#pragma region Alterations
/// <summary>
/// Moves along WORLD axes
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	matrixDirty = true;
}

/// <summary>
/// Moves along RELATIVE axes (our own)
/// </summary>
/// <param name="x">x to move by</param>
/// <param name="y">y to move by</param>
/// <param name="z">z to move by</param>
void Transform::MoveRelative(float x, float y, float z)
{
	XMVECTOR relPos = XMVectorSet(x, y, z, 0);
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMVECTOR dir = XMVector3Rotate(relPos, quat);

	XMVECTOR temp = XMLoadFloat3(&position);
	XMStoreFloat3(&position, temp + dir);
	matrixDirty = true;

}

void Transform::LookAt(DirectX::XMVECTOR pos, DirectX::XMVECTOR dir)
{
	// XMMATRIX tempRot = XMMatrixLookAtLH(pos, pos + dir, XMVectorSet(0, 1, 0, 0));
	
	XMMATRIX tempRot = XMMatrixLookToLH(pos, dir, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	return;
	XMVECTOR quatRot = XMQuaternionRotationMatrix(tempRot);
	XMFLOAT3 tempFloatRot;
	XMStoreFloat3(&rotation, quatRot);
	
	matrixDirty = true;
	return;

	XMMATRIX tempPos = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX tempScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX world = tempScale * tempRot * tempPos;
	// wMat = wMat * tempMat;
	XMStoreFloat4x4(&worldMatrix, world);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;
	matrixDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	matrixDirty = true;
}
#pragma endregion