#include "Entity.h"
#include "BufferStructs.h"
#include "Helpers.h"
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
		rb = new Rigidbody(mesh->GetVertices(), &transform, _isDynamic);
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
		// tPos = XMFLOAT3(0,0,0);

		// applying rotation to offset
		XMVECTOR tempOffset = XMLoadFloat3(&tPos);
		XMFLOAT3 rotation = transform.GetPitchYawRoll();
		XMVECTOR quat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		tempOffset = XMVector3Rotate(tempOffset, quat);
		XMStoreFloat3(&tPos, tempOffset);


		XMFLOAT3 cPos = rb->GetCenterGlobal();
		// The rigidbody has moved, so we move the mesh to follow it
		transform.SetPosition(cPos.x + tPos.x, cPos.y+ tPos.y, cPos.z + tPos.z);
		ResolveInputs(deltaTime);
	}
}

#pragma region ResolveInputs
// gathers inputs and applies the appripriate force
void Entity::ResolveInputs(float deltaTime)
{
	// if w, apply force on forward axis
	if (GetAsyncKeyState('W') & 0x8000)
	{
		rb->HandleDrive(1);
	}

	// if S, apply force on (-1)forward axis
	if (GetAsyncKeyState('S') & 0x8000)
	{
		rb->HandleDrive(-1);
	}

	// if a, negative rotation on the Y axis
	if (GetAsyncKeyState('A') & 0x8000)
	{
		// negative rotation on Y axis by the turn radius
		// transform.Rotate(0.0f, (-5 * deltaTime), 0.0f);
		rb->HandleSteering(-1, deltaTime);
	}

	// if d, positive rotation on the Y axis
	if (GetAsyncKeyState('D') & 0x8000)
	{
		// positive rotation on Y axis by the turn radius
		// transform.Rotate(0.0f, (5 * deltaTime), 0.0f);
		rb->HandleSteering(1, deltaTime);
	}
}
#pragma endregion

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> ctx, Camera* cam, char type)
{

	SimpleVertexShader* vs = material->GetVS(); //Simplifies next few lines
	SimplePixelShader* ps = material->GetPS();
	vs->SetShader();
	ps->SetShader();
	if (type == 'S')
	{
		vs->SetMatrix4x4("view", cam->GetViewMatrix());
		vs->SetMatrix4x4("proj", cam->GetProjectionMatrix());
		// vs->CopyAllBufferData();

		ps->SetShaderResourceView("EnvironmentMap", material->GetSRV());
		// ps->SetSamplerState("samplerOptions", material->GetSampler());
	}
	else
	{
		vs->SetFloat4("tint", material->GetTint());
		vs->SetMatrix4x4("world", transform.GetWorldMatrix());
		vs->SetMatrix4x4("view", cam->GetViewMatrix());
		vs->SetMatrix4x4("proj", cam->GetProjectionMatrix());

		ps->SetFloat3("camPos", cam->GetTransform().GetPosition());
		ps->SetFloat("spec", material->GetSpecIntensity());
		ps->SetShaderResourceView("diffuseTexture", material->GetSRV());

		if (material->GetNormalMap())
		{
			ps->SetShaderResourceView("normalMap", material->GetNormalMap());
		}
	}
	// Not needed until there's texture data
	ps->SetSamplerState("samplerOptions", material->GetSampler());

	// Copy buffer data
	vs->CopyAllBufferData();
	ps->CopyAllBufferData();

	// Set buffers before drawing
	UINT stride = sizeof(Vertex);
	// if (type != 'S') stride = sizeof(MinimumVertex);
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
	// XMFLOAT3 s = transform.GetScale();
	temp.SetScale(hw.x * 2, hw.y * 2, hw.z * 2);
	XMFLOAT3 pos = rb->GetCenterGlobal();
	temp.SetPosition(pos.x, pos.y, pos.z);

	vs->SetFloat4("tint", rb->tint);
	vs->SetMatrix4x4("world", temp.GetWorldMatrix());
	vs->SetMatrix4x4("view", cam->GetViewMatrix());
	vs->SetMatrix4x4("proj", cam->GetProjectionMatrix());



	// Copy buffer data
	vs->CopyAllBufferData();
	// ps->CopyAllBufferData();

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

void Entity::DrawDebugObject(Microsoft::WRL::ComPtr<ID3D11DeviceContext> ctx, Camera* cam, Mesh* colliderMesh, SimpleVertexShader* vs, SimplePixelShader* ps)
{
	// Set shaders in Entity Draw -> INEFFICIENT, WILL IMPLEMENT RENDER ORDER IN FUTURE!
	vs->SetShader();
	ps->SetShader();


	Transform temp = transform;
	temp.SetScale(1.0f, 1.0f, 1.0f);
	XMFLOAT3 pos = rb->GetMaxGlobal();
	// pos = rb->myTransform.GetPosition();
	XMFLOAT3 poffset = rb->GetParentalOffset();
	poffset = XMFLOAT3(0, 0, 0);
	temp.SetPosition(pos.x + poffset.x, pos.y + poffset.y, pos.z + poffset.z);

	vs->SetFloat4("tint", material->GetTint());
	vs->SetMatrix4x4("world", temp.GetWorldMatrix());
	vs->SetMatrix4x4("view", cam->GetViewMatrix());
	vs->SetMatrix4x4("proj", cam->GetProjectionMatrix());


	// Copy buffer data
	vs->CopyAllBufferData();
	// ps->CopyAllBufferData();

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

void Entity::DrawHandles(Microsoft::WRL::ComPtr<ID3D11DeviceContext> ctx, Camera* cam, Mesh* colliderMesh, SimpleVertexShader* vs, SimplePixelShader* ps)
{
	// Set shaders in Entity Draw->INEFFICIENT, WILL IMPLEMENT RENDER ORDER IN FUTURE!
	vs->SetShader();
	ps->SetShader();


	Transform temp = transform;
	temp.SetScale(5, 5, 5);

	vs->SetFloat4("tint", XMFLOAT4(1,1,0,0));
	vs->SetMatrix4x4("world", temp.GetWorldMatrix());
	vs->SetMatrix4x4("view", cam->GetViewMatrix());
	vs->SetMatrix4x4("proj", cam->GetProjectionMatrix());


	// Copy buffer data
	vs->CopyAllBufferData();
	// ps->CopyAllBufferData();

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

Rigidbody* Entity::GetRigidBody()
{
	return rb;
}
