#include "Material.h"

using namespace DirectX;
Material::Material(SimplePixelShader* ps, SimpleVertexShader* vs, XMFLOAT4 tint,
	float spec,
	ID3D11ShaderResourceView* _srv,
	ID3D11SamplerState* _sampler,
	ID3D11ShaderResourceView* _nMap)
{
	this->ps = ps;
	this->vs = vs;
	this->tint = tint;
	srv = _srv;
	sampler = _sampler;
	specIntensity = spec;
	nMap = _nMap;
}

DirectX::XMFLOAT4 Material::GetTint()
{
	return tint;
}

void Material::SetTint(DirectX::XMFLOAT4 tint)
{
	this->tint = tint;
}

SimplePixelShader* Material::GetPS()
{
	return ps;
}

SimpleVertexShader* Material::GetVS()
{
	return vs;
}

ID3D11ShaderResourceView* Material::GetSRV()
{
	return srv;
}

ID3D11SamplerState* Material::GetSampler()
{
	return sampler;
}

ID3D11ShaderResourceView* Material::GetNormalMap()
{
	return nMap;
}

float Material::GetSpecIntensity()
{
	return specIntensity;
}
