#include "Entity.h"
#include "BufferStructs.h"
using namespace DirectX;
Entity::Entity(Mesh* m, Material* mat, Transform t, bool physics, bool _isDynamic)
{
	mesh = m;
	material = mat;
	transform = t;
	phyicsObject = physics;
	if (phyicsObject)
	{
		// Instantiate as Physics Object
		rb = new Rigidbody(mesh->GetVertices(), transform, _isDynamic);
		isDynamic = _isDynamic;

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

void Entity::Update(float deltaTime, float totalTime)
{
	if (phyicsObject && isDynamic)
	{
		rb->Update(deltaTime, totalTime);
		// ADD OVERLOAD TO TRANSFORM: AddOffset
		XMFLOAT3 tPos = rb->GetParentalOffset();
		XMFLOAT3 cPos = rb->GetCenterGlobal();
		transform.SetPosition(cPos.x + tPos.x, cPos.y+ tPos.y, cPos.z + tPos.z);
	}
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


void Entity::DrawCollider(Microsoft::WRL::ComPtr<ID3D11DeviceContext> ctx, Camera* cam, Mesh* colliderMesh, 
	SimpleVertexShader* vs, SimplePixelShader* ps)
{
	// Set shaders in Entity Draw -> INEFFICIENT, WILL IMPLEMENT RENDER ORDER IN FUTURE!
	vs->SetShader();
	ps->SetShader();


	Transform temp = transform;
	XMFLOAT3 hw = rb->GetHalfWidth();
	XMFLOAT3 s = transform.GetScale();
	temp.SetScale(hw.x * s.x * 2, hw.y * s.y * 2, hw.z * s.z * 2);
	XMFLOAT3 pos = rb->GetCenterGlobal();
	temp.SetPosition(pos.x, pos.y, pos.z);

	vs->SetFloat4("tint", material->GetTint());
	vs->SetMatrix4x4("world", temp.GetWorldMatrix());
	vs->SetMatrix4x4("view", cam->GetViewMatrix());
	vs->SetMatrix4x4("proj", cam->GetProjectionMatrix());

	ps->SetFloat3("camPos", cam->GetTransform().GetPosition());
	ps->SetFloat("spec", material->GetSpecIntensity());
	ps->SetSamplerState("samplerOptions", material->GetSampler());
	ps->SetShaderResourceView("diffuseTexture", material->GetSRV());


	// Copy buffer data
	vs->CopyAllBufferData();
	ps->CopyAllBufferData();

	// Set buffers before drawing
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ctx->IASetVertexBuffers(0, 1, colliderMesh->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	ctx->IASetIndexBuffer(colliderMesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	// Actual Draw call
	ctx->DrawIndexed(
		colliderMesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}

bool Entity::IsPhysicsObject()
{
	return phyicsObject;
}
