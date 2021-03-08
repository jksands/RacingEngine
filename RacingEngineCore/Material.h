#pragma once
#include <DirectXMath.h>
#include "DXCore.h"
#include "SimpleShader.h"
// Basic Material class -> Will be updated according to our engine!
class Material
{
private:
	DirectX::XMFLOAT4 tint;
	SimplePixelShader* ps;
	SimpleVertexShader* vs;
	ID3D11ShaderResourceView* srv;
	ID3D11ShaderResourceView* nMap;
	ID3D11SamplerState* sampler;
	float specIntensity;


public:
	Material(SimplePixelShader* ps, 
		SimpleVertexShader* vs, 
		DirectX::XMFLOAT4 tint, 
		float specIntensity = 0,
		ID3D11ShaderResourceView* _srv = nullptr,
		ID3D11SamplerState* _sampler = nullptr,
		ID3D11ShaderResourceView* _nMap = nullptr);
	DirectX::XMFLOAT4 GetTint();
	void SetTint(DirectX::XMFLOAT4 tint);
	SimplePixelShader* GetPS();
	SimpleVertexShader* GetVS();
	ID3D11ShaderResourceView* GetSRV();
	ID3D11SamplerState* GetSampler();
	ID3D11ShaderResourceView* GetNormalMap();

	float GetSpecIntensity();

};

