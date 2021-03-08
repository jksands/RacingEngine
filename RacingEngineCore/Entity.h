#pragma once
#include "DXCore.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"
#include "Rigidbody.h"
#include <d3dcompiler.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
class Entity
{
public:
	Entity(Mesh* m, Material* mat, Transform t = Transform(), bool physicsObject = false);
	Mesh* GetMesh();
	Transform* GetTransform();
	// For time being, Entities handle their own Draw.  May be updated in the future with an addition of a renderer
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> ctx, Camera* cam, char c = ' ');

private:
	Transform transform;
	Mesh* mesh = nullptr;
	Material* material = nullptr;
	Rigidbody* rb;
};

