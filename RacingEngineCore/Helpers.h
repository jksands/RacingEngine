#pragma once
#include <DirectXMath.h>

#pragma region Helpers
// helpers

#pragma region DirectX::XMFLOAT3 Maths
// Adding to DirectX::XMFLOAT3's
inline DirectX::XMFLOAT3 AddFloat3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
	DirectX::XMFLOAT3 temp;
	temp.x = a.x + b.x;
	temp.y = a.y + b.y;
	temp.z = a.z + b.z;
	return temp;
}
// Subtracting 2 DirectX::XMFLOAT3's
inline DirectX::XMFLOAT3 SubFloat3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
	DirectX::XMFLOAT3 temp;
	temp.x = a.x - b.x;
	temp.y = a.y - b.y;
	temp.z = a.z - b.z;
	return temp;
}
// Divides a DirectX::XMFLOAT3 by a scalar
inline DirectX::XMFLOAT3 DivFloat3(DirectX::XMFLOAT3 float3, float scalar)
{
	DirectX::XMFLOAT3 temp;
	temp.x = float3.x / scalar;
	temp.y = float3.y / scalar;
	temp.z = float3.z / scalar;
	return temp;
}
/// <summary>
/// 
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <returns></returns>
inline DirectX::XMFLOAT3 MultFloat3(DirectX::XMFLOAT3 float3, float scalar)
{
	DirectX::XMFLOAT3 temp;
	temp.x = float3.x * scalar;
	temp.y = float3.y * scalar;
	temp.z = float3.z * scalar;
	return temp;
}
inline float MagFloat3(DirectX::XMFLOAT3 float3)
{
	// a^2 + b^2 + c^2 = d^2
	// d = sqrt(a^2 + b^2 + c^2)
	return abs(sqrtf(powf(float3.x, 2.0f) + powf(float3.y, 2.0f) + powf(float3.z, 2.0f)));
}
#pragma endregion

#pragma endregion