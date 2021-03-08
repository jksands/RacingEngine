#include "Rigidbody.h"
#include "Transform.h"
using namespace DirectX;
// constructor & big 3
/*
		* Constructor
		* Params:
		*	std::vector<XMFLOAT3> pointList: the points to make the rigidbody out of
		* Returns: an instance of the class
		*/
Rigidbody::Rigidbody(Vertex vertices[], Transform incomingTransform)
{
	// if there are no vertices, return out
	int vertexCount = sizeof(vertices) / sizeof(vertices[0]);
	if (vertexCount == 0)
	{
		return;
	}
	// set min and max before finding the true min/max
	maxLocal = vertices[0].Position;
	minLocal = vertices[0].Position;
	// finding the true min and max
	for (int i = 0; i < vertexCount; i++)
	{
		// x value
		if (vertices[i].Position.x > maxLocal.x)
		{
			maxLocal.x = vertices[0].Position.x;
		}
		else if (vertices[i].Position.x < minLocal.x)
		{
			minLocal.x = vertices[i].Position.x;
		}
		// y value
		if (vertices[i].Position.y > maxLocal.y)
		{
			maxLocal.y = vertices[0].Position.y;
		}
		else if (vertices[i].Position.y < minLocal.y)
		{
			minLocal.y = vertices[i].Position.y;
		}
		// z value
		if (vertices[i].Position.z > maxLocal.z)
		{
			maxLocal.z = vertices[0].Position.z;
		}
		else if (vertices[i].Position.x < minLocal.z)
		{
			minLocal.z = vertices[i].Position.z;
		}
	}
	// since the model matrix is the identity, then the global and local should be the same
	minGlobal = minLocal;
	maxGlobal = maxLocal;
	// find the centre
	centerLocal = DivFloat3(AddFloat3(maxLocal, minLocal), 2.0f);
	// half width is dist between min and max over 2
	halfWidthOBB = DivFloat3(SubFloat3(maxLocal, minLocal), 2.0f);
	// find the radius by distance bettwen centre and either min or max
	radius = MagFloat3(SubFloat3(maxLocal, centerLocal));
	rotQuat = XMQuaternionIdentity();
	ARBBVisible = true;
	myTransform = incomingTransform;
	localToGlobalMat = myTransform.GetWorldMatrix();
}

/*
		* Copy Constructor
		* Params:
		*	Rigidbody const& incoming: incoming instance to copy
		* Returns: an instance of the class
		*/
Rigidbody::Rigidbody(Rigidbody const& incoming)
{
	boundingSphereVisible = incoming.boundingSphereVisible;
	OBBVisible = incoming.OBBVisible;
	ARBBVisible = incoming.ARBBVisible;
	radius = incoming.radius;
	collidingColour = incoming.collidingColour;
	notCollidingColour = incoming.notCollidingColour;
	centerLocal = incoming.centerLocal;
	minLocal = incoming.minLocal;
	maxLocal = incoming.maxLocal;
	centerGlobal = incoming.centerGlobal;
	minGlobal = incoming.minGlobal;
	maxGlobal = incoming.minGlobal;
	halfWidthOBB = incoming.halfWidthOBB;
	sizeARBB = incoming.sizeARBB;
	localToGlobalMat = incoming.localToGlobalMat;
	collidingCount = incoming.collidingCount;
	collidingArray = incoming.collidingArray;
	accel = incoming.accel;
	pos = incoming.pos;
	rotQuat = incoming.rotQuat;
	size = incoming.size;
	vel = incoming.vel;
	mass = incoming.mass;
	myTransform = incoming.myTransform;
}

/*
		* Copy Assignment Operator
		* Params:
		*	Rigidbody const& incoming: incoming instance to copy
		* Returns: an instance of the class
		*/
		/*
		Rigidbody& Rigidbody::operator=(Rigidbody& incoming)
		{
			boundingSphereVisible = incoming.boundingSphereVisible;
			OBBVisible = incoming.OBBVisible;
			ARBBVisible = incoming.ARBBVisible;
			radius = incoming.radius;
			collidingColour = incoming.collidingColour;
			notCollidingColour = incoming.notCollidingColour;
			centreLocal = incoming.centreLocal;
			minLocal = incoming.minLocal;
			maxLocal = incoming.maxLocal;
			centreGlobal = incoming.centreGlobal;
			minGlobal = incoming.minGlobal;
			maxGlobal = incoming.minGlobal;
			halfWidthOBB = incoming.halfWidthOBB;
			sizeARBB = incoming.sizeARBB;
			localToGlobalMat = incoming.localToGlobalMat;
			collidingCount = incoming.collidingCount;
			collidingArray = incoming.collidingArray;
			accel = incoming.accel;
			pos = incoming.pos;
			rotQuat = incoming.rotQuat;
			size = incoming.size;
			vel = incoming.vel;
			mass = incoming.mass;

			myTransform = incoming.myTransform;
		}
		*/
		/*
		* Destructor
		* Params: N/A
		* Returns: N/A
		*/
Rigidbody::~Rigidbody()
{
	ClearCollisionList();
}
// getters and setters
// bounding sphere visible
bool Rigidbody::GetBoundingSphereVisible() { return boundingSphereVisible; }
void Rigidbody::SetBoundingSphereVisible(bool value) { boundingSphereVisible = value; }
// OBB visible
bool Rigidbody::GetOBBVisible() { return OBBVisible; }
void Rigidbody::SetOBBVisible(bool value) { OBBVisible = value; }
// Axis-Realigned Bounding Box visibility
bool Rigidbody::GetARBBVisible() { return ARBBVisible; }
void Rigidbody::SetARBBVisible(bool value) { ARBBVisible = value; }
// returns radius
float Rigidbody::GetRadius() { return radius; }
// colliding colour
XMFLOAT3 Rigidbody::GetCollidingColour() { return collidingColour; }
void Rigidbody::SetCollidingColour(XMFLOAT3 value) { collidingColour = value; }
// not colliding colour
XMFLOAT3 Rigidbody::GetNotCollidingCOlour() { return notCollidingColour; }
void Rigidbody::SetNotCollidingColour(XMFLOAT3 value) { notCollidingColour = value; }

// local variables
XMFLOAT3 Rigidbody::GetCentreLocal() { return centerLocal; }
XMFLOAT3 Rigidbody::GetMinLocal() { return minLocal; }
XMFLOAT3 Rigidbody::GetMaxLocal() { return maxLocal; }
//global variables
XMFLOAT3 Rigidbody::GetCenterGlobal() { return centerGlobal; }
XMFLOAT3 Rigidbody::GetMinGlobal() { return minGlobal; }
XMFLOAT3 Rigidbody::GetMaxGlobal() { return maxGlobal; }
// halfWidth 
XMFLOAT3 Rigidbody::GetHalfWidth() { return halfWidthOBB; }
// matrix model
XMFLOAT4X4 Rigidbody::GetModelMatrix() { return localToGlobalMat; }
void Rigidbody::SetModelMatrix(XMFLOAT4X4 modelMatrix) { localToGlobalMat = modelMatrix; }
// collision array stuff
Rigidbody* Rigidbody::GetCollidingArray() { return collidingArray; }
int Rigidbody::GetCollidingCount() { return collidingCount; }
// position getter and setter
void Rigidbody::SetPosition(XMFLOAT3 incomingPos) { pos = incomingPos; }
XMFLOAT3 Rigidbody::GetPosition() { return pos; }
// rotation getter and setter
void Rigidbody::SetRotation(XMVECTOR incomingRot) { rotQuat = incomingRot; }
XMVECTOR Rigidbody::GetRotation() { return rotQuat; }
// size getter and setter
void Rigidbody::SetSize(XMFLOAT3 incomingSize) { size = incomingSize; }
XMFLOAT3 Rigidbody::GetSize() { return size; }
// velocity getter and setter
void Rigidbody::SetVelocity(XMFLOAT3 incomingVel) { vel = incomingVel; }
XMFLOAT3 Rigidbody::GetVelocity() { return vel; }
// mass getter and setter
void Rigidbody::SetMass(float incomingMass) { mass = incomingMass; }
float Rigidbody::GetMass() { return mass; }

// other methods
		/*
		* Clear the colliding list
		* Params:
		*	None
		* Returns: None
		*/
void Rigidbody::ClearCollisionList()
{
	collidingCount = 0;
	if (collidingArray)
	{
		delete[] collidingArray;
		collidingArray = nullptr;
	}
}
/*
* Checks to see if this rigidbody is colliding with the incoing rigidbody
* Params:
*	Rigidbody* incoming -- rigidbody to check collisions against
* Returns: boolean telling us if the two are colliding
*/
bool Rigidbody::IsColliding(Rigidbody* incoming)
{
	// sphere collision precheck
	bool isColliding;
	if (MagFloat3(SubFloat3(centerGlobal, incoming->GetCenterGlobal())) <= radius + incoming->GetRadius())
	{
		isColliding = true;
	}
	else
	{
		isColliding = false;
	}
	// run sat/ARBB now
	if (isColliding)
	{
		//if (SAT(incoming) != 0)
		if (ARBBCheck(incoming) == false)
		{
			isColliding = false;
		}
	}
	if (isColliding)
	{
		// add to collision list here
	}
	else
	{
		// remove from collision list here
	}
	return isColliding;
}
/*
* Uses ARBB collision detections to return if this is colliding with another giidbody
* params: incoing rigidbody
* returns true if colliding false if not collidiing
*/
bool Rigidbody::ARBBCheck(Rigidbody* incoming)
{
	// if max > their min  or  their max > min
	// x axis
	if ((maxGlobal.x > incoming->GetMinGlobal().x)
		|| (incoming->GetMaxGlobal().x > minGlobal.x))
	{
		return true;
	}
	// y axis
	if ((maxGlobal.y > incoming->GetMinGlobal().y)
		|| (incoming->GetMaxGlobal().y > minGlobal.y))
	{
		return true;
	}
	// z axis
	if ((maxGlobal.z > incoming->GetMinGlobal().z)
		|| (incoming->GetMaxGlobal().z > minGlobal.z))
	{
		return true;
	}
	// if those fail, then no collide
	return false;
}
// helpers
// Adding to XMFLOAT3's
XMFLOAT3 Rigidbody::AddFloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	XMFLOAT3 temp;
	temp.x = a.x + b.x;
	temp.y = a.y + b.y;
	temp.z = a.z + b.z;
	return temp;
}
// Subtracting 2 XMFLOAT3's
XMFLOAT3 Rigidbody::SubFloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	XMFLOAT3 temp;
	temp.x = a.x - b.x;
	temp.y = a.y - b.y;
	temp.z = a.z - b.z;
	return temp;
}
// Divides a XMFLOAT3 by a scalar
XMFLOAT3 Rigidbody::DivFloat3(XMFLOAT3 float3, float scalar)
{
	XMFLOAT3 temp;
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
XMFLOAT3 Rigidbody::MultFloat3(XMFLOAT3 float3, float scalar)
{
	XMFLOAT3 temp;
	temp.x = float3.x * scalar;
	temp.y = float3.y * scalar;
	temp.z = float3.z * scalar;
	return temp;
}
float Rigidbody::MagFloat3(XMFLOAT3 float3)
{
	// a^2 + b^2 + c^2 = d^2
	// d = sqrt(a^2 + b^2 + c^2)
	return sqrtf(powf(float3.x, 2.0f) + powf(float3.y, 2.0f) + powf(float3.z, 2.0f));
}