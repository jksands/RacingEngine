#include "Entity.h"
#include "BufferStructs.h"
using namespace DirectX;
Entity::Entity(Mesh* m, Material* mat, Transform t, bool physics)
{
	mesh = m;
	material = mat;
	transform = t;
	if (physics)
	{
		// Instantiate as Physics Object
		rb = new Rigidbody(mesh->GetVertices(), transform);
	}
}

Mesh* Entity::GetMesh()
{
	return mesh;
}

Transform* Entity::GetTransform()
{
	return &transform;
}

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> ctx, Camera* cam, char type)
{
	// Set shaders in Entity Draw -> INEFFICIENT, WILL IMPLEMENT RENDER ORDER IN FUTURE!
	material->GetVS()->SetShader();
	material->GetPS()->SetShader();

	SimpleVertexShader* vs = material->GetVS(); //Simplifies next few lines
	SimplePixelShader* ps = material->GetPS();
	if (type == 'S')
	{
		vs->SetMatrix4x4("view", cam->GetViewMatrix());
		vs->SetMatrix4x4("proj", cam->GetProjectionMatrix());
		// vs->CopyAllBufferData();

		ps->SetShaderResourceView("EnvironmentMap", material->GetSRV());
		ps->SetSamplerState("BasicSampler", material->GetSampler());
	}
	else
	{
		vs->SetFloat4("tint", material->GetTint());
		vs->SetMatrix4x4("world", transform.GetWorldMatrix());
		vs->SetMatrix4x4("view", cam->GetViewMatrix());
		vs->SetMatrix4x4("proj", cam->GetProjectionMatrix());

		ps->SetFloat3("camPos", cam->GetTransform().GetPosition());
		ps->SetFloat("spec", material->GetSpecIntensity());
		ps->SetSamplerState("samplerOptions", material->GetSampler());
		ps->SetShaderResourceView("diffuseTexture", material->GetSRV());
		if (material->GetNormalMap())
		{
			ps->SetShaderResourceView("normalMap", material->GetNormalMap());
		}
	}

	// Copy buffer data
	vs->CopyAllBufferData();
	ps->CopyAllBufferData();

	// Set buffers before drawing
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ctx->IASetVertexBuffers(0, 1, mesh->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	ctx->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	// Actual Draw call
	ctx->DrawIndexed(
		mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}
