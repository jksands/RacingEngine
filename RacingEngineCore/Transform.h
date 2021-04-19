#pragma once
#include <DirectXMath.h>

class Transform
{
private:

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;
	bool matrixDirty = false;
public:

	Transform(
		DirectX::XMFLOAT3 _position = DirectX::XMFLOAT3(0,0,0), 
		DirectX::XMFLOAT3 _scale = DirectX::XMFLOAT3(1,1,1), 
		DirectX::XMFLOAT3 _rotation = DirectX::XMFLOAT3(0,0,0));
	Transform(Transform* t);
	
	// Setters
	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);
	//Getters
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	// Transformers (More than meets the eye)
	void MoveAbsolute(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);
	void MoveRelative(float x, float y, float z);
	void LookAt(DirectX::XMVECTOR pos, DirectX::XMVECTOR dir);

};

