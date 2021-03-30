#include "Rigidbody.h"
#include "Transform.h"
#include "EntityManager.h"
using namespace DirectX;
// constructor & big 3

#pragma region Big3
/*
* Constructor
* Params:
*	std::vector<XMFLOAT3> pointList: the points to make the rigidbody out of
* Returns: an instance of the class
*/
Rigidbody::Rigidbody(std::vector<Vertex> vertices, Transform incomingTransform, bool _isDynamic)
{
	// if there are no vertices, return out
	int vertexCount = vertices.size();
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
			maxLocal.x = vertices[i].Position.x;
		}
		else if (vertices[i].Position.x < minLocal.x)
		{
			minLocal.x = vertices[i].Position.x;
		}
		// y value
		if (vertices[i].Position.y > maxLocal.y)
		{
			maxLocal.y = vertices[i].Position.y;
		}
		else if (vertices[i].Position.y < minLocal.y)
		{
			minLocal.y = vertices[i].Position.y;
		}
		// z value
		if (vertices[i].Position.z > maxLocal.z)
		{
			maxLocal.z = vertices[i].Position.z;
		}
		else if (vertices[i].Position.z < minLocal.z)
		{
			minLocal.z = vertices[i].Position.z;
		}
	}

	// getting transform and setting local to global matrix
	myTransform = incomingTransform;
	localToGlobalMat = myTransform.GetWorldMatrix();

	// half width is dist between min and max over 2
	halfWidthOBB = DivFloat3(SubFloat3(maxLocal, minLocal), 2.0f);
	XMFLOAT3 tempScale = myTransform.GetScale();
	halfWidthOBB = XMFLOAT3(halfWidthOBB.x * tempScale.x, halfWidthOBB.y * tempScale.y, halfWidthOBB.z * tempScale.z);

	// setting max global and min global
	// min
	minGlobal = GetMinGlobal();

	// max
	maxGlobal = GetMaxGlobal();
	// maxGlobal = MultFloat3(maxGlobal, incomingTransform.GetScale().x);

	// find the centre
	centerLocal = DivFloat3(AddFloat3(maxLocal, minLocal), 2.0f);
	// find the radius by distance bettwen centre and either min or max
	radius = MagFloat3(SubFloat3(maxGlobal, GetCenterGlobal()));
	rotQuat = XMQuaternionIdentity();
	ARBBVisible = true;
	vel = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// calculate center global
	XMMATRIX mat = XMLoadFloat4x4(&localToGlobalMat);
	XMVECTOR pos = XMLoadFloat3(&centerLocal);
	XMStoreFloat3(&centerGlobal, XMVector3Transform(pos, mat));
	// centerGlobal = GetCenterGlobal();

	// After calculating center global, get the offset between US and the parent center
	offset = SubFloat3(myTransform.GetPosition(), centerGlobal);

	isDynamic = _isDynamic;
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

#pragma endregion

#pragma region Getters and Setters
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
XMFLOAT3 Rigidbody::GetCenterGlobal() { 

	// calculate center global
	XMMATRIX mat = XMLoadFloat4x4(&localToGlobalMat);
	XMFLOAT4 tP = XMFLOAT4(centerLocal.x, centerLocal.y, centerLocal.z, 1.0f);
	XMVECTOR p = XMLoadFloat4(&tP);
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, XMVector3Transform(p, mat));
	return temp; }
XMFLOAT3 Rigidbody::GetMinGlobal() { return AddFloat3(GetCenterGlobal(), MultFloat3(halfWidthOBB, -1)); }
//{
//	// calculate center global
//	XMMATRIX mat = XMLoadFloat4x4(&localToGlobalMat);
//	XMFLOAT3 temp = AddFloat3(offset, minLocal);
//	XMVECTOR p = XMLoadFloat3(&minLocal);
//
//	XMStoreFloat3(&minGlobal, XMVector3Transform(p, mat));
//
//	return minGlobal;
//} 

XMFLOAT3 Rigidbody::GetMaxGlobal() { return AddFloat3(GetCenterGlobal(), halfWidthOBB); }
//{ // calculate center global
//	XMMATRIX mat = XMLoadFloat4x4(&localToGlobalMat);
//	XMVECTOR p = XMLoadFloat3(&maxLocal);
//
//	XMStoreFloat3(&maxGlobal, XMVector3Transform(p, mat));
//
//	return maxGlobal;
//} 

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

DirectX::XMFLOAT3 Rigidbody::GetParentalOffset()
{
	return offset;
}

#pragma endregion

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

#pragma region IsColliding
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
	if (MagFloat3(SubFloat3(GetCenterGlobal(), incoming->GetCenterGlobal())) <= radius + incoming->GetRadius())
	{
		isColliding = true;
		isGrounded = true;
	}
	else
	{
		isColliding = false;
		isGrounded = false;
	}

	// run SAT now
	if (isColliding)
	{
		if (SAT(incoming) == 1)
		// if (ARBBCheck(incoming) == false)
		{
			isColliding = false;
			isGrounded = false;
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

#pragma endregion

#pragma region ApplyForce and Friction and ApplyGravity
// applies friction by multiplying vel by a number less than 1
void  Rigidbody::ApplyFriction(float incomingFrictionCoefficient) // dont call unless mioving   --  overcoming static coefficient of friction
{
	// makes sure it's not too low
	if (incomingFrictionCoefficient < 0.0001f)
	{
		incomingFrictionCoefficient = 0.00001f;
	}

	// coudl probs change these to just the x and z components. Don't need to update the y
	// vel = MultFloat3(vel, 1.0f - incomingFrictionCoefficient);
	vel.x *= (1 - incomingFrictionCoefficient);
	vel.z *= (1 - incomingFrictionCoefficient);

	if (MagFloat3(vel) < 0.0001f)
	{
		vel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
}

// applies a force to the object
void  Rigidbody::ApplyForce(XMFLOAT3 incomingForce)
{
	if (mass < 0.01f)
	{
		mass = 0.01f;
	}

	//f = m*a -> a = f/m
	accel = AddFloat3(accel, DivFloat3(incomingForce, mass));
}

void Rigidbody::ApplyGrav(float gravity)
{
	accel.y += (-gravity);
}

#pragma endregion

#pragma region ARBB

/*
* Uses ARBB collision detections to return if this is colliding with another giidbody
* params: incoing rigidbody
* returns true if colliding false if not collidiing
*/
bool Rigidbody::ARBBCheck(Rigidbody* incoming)
{
	XMFLOAT3 myMax = GetMaxGlobal();
	XMFLOAT3 youMax = incoming->GetMaxGlobal(); 
	XMFLOAT3 myMin = GetMinGlobal();
	XMFLOAT3 youMin = incoming->GetMinGlobal();

	// if max > their min  or  their max > min
	if ((myMax.x > youMin.x) // x axis
		&& (youMax.x > myMin.x)
		&& (myMax.y > youMin.y) // y axis
		&& (youMax.y > myMin.y)
		&& (myMax.z > youMin.z) // z axis
		&& (youMax.z > myMin.z))
	{
		return true;
	}

	// if those fail, then no collide
	return false;
}

#pragma endregion

#pragma region SAT
/*
	* does the SAT collision check
	* Arguments: the incoming rigidbody to collide with
	* Returns: 1 if no collision 0 if collision
	*/
int Rigidbody::SAT(Rigidbody* incoming)
{
	float myRadius;
	float yourRadius;

	XMFLOAT3X3 rotationMatrix;
	XMFLOAT3X3 absRotMax;

	XMFLOAT4 xAxisFloat = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 yAxisFloat = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	XMFLOAT4 zAxisFloat = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);

	XMVECTOR xAxis = XMLoadFloat4(&xAxisFloat);
	XMVECTOR yAxis = XMLoadFloat4(&yAxisFloat);
	XMVECTOR zAxis = XMLoadFloat4(&zAxisFloat);
	
	XMMATRIX myWorldMat = XMLoadFloat4x4(&localToGlobalMat);
	XMMATRIX yourWorldMat = XMLoadFloat4x4(&incoming->localToGlobalMat);
	// XMStoreFloat3(&centerGlobal, XMVector3Transform(p, mat));

	
	// get all the axes for each object
	XMVECTOR myAxes[] = {
		XMVector3Normalize(XMVector3Transform(xAxis, myWorldMat)),
		XMVector3Normalize(XMVector3Transform(yAxis, myWorldMat)),
		XMVector3Normalize(XMVector3Transform(zAxis, myWorldMat))
	};
	XMVECTOR yourAxes[] = {
		XMVector3Normalize(XMVector3Transform(xAxis, yourWorldMat)),
		XMVector3Normalize(XMVector3Transform(yAxis, yourWorldMat)),
		XMVector3Normalize(XMVector3Transform(zAxis, yourWorldMat))
	};

	// compute rotation matrix: incoming in my coordinate frame
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// rotMat at i,j is dot of myAxes[i], yourAxes[j]
			rotationMatrix.m[i][j] = XMVectorGetX(XMVector3Dot(myAxes[i], yourAxes[j]));
		}
	}

	// compute the translation vector
	// incoming centre global - my center global
	XMVECTOR transVec = XMLoadFloat3(&SubFloat3(incoming->GetCenterGlobal(),GetCenterGlobal()));

	XMFLOAT3 myGlob = GetCenterGlobal();
	XMFLOAT3 yourGlob = incoming->GetCenterGlobal();

	// bring tanslation vector into a's coord frame
	float tX = XMVectorGetX(XMVector3Dot(transVec, myAxes[0]));
	float tY = XMVectorGetY(XMVector3Dot(transVec, myAxes[1]));
	float tZ = XMVectorGetZ(XMVector3Dot(transVec, myAxes[2]));
	XMFLOAT3 tempF3 = XMFLOAT3(tX, tY, tZ);
	transVec = XMLoadFloat3(&tempF3);

	// i think i need absR bc parrallell makes math sadger
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			absRotMax.m[i][j] = (float)abs(rotationMatrix.m[i][j] + EPSILON);
		}
	}

	// testing the axes

	// L = A0, l + A1, L = A2
	for (int i = 0; i < 3; i++)
	{
		float temp;
		// if's to set up x, y, or z components because DirectX won't let me use indexors
		// didn't use XMVectorGetIndex b/c performance drops
		if (i == 0)
		{
			myRadius = GetHalfWidth().x;
			temp = XMVectorGetX(transVec);
		}
		else if (i == 1)
		{
			myRadius = GetHalfWidth().y;
			temp = XMVectorGetY(transVec);
		}
		else if (i == 2)
		{
			myRadius = GetHalfWidth().z;
			temp = XMVectorGetZ(transVec);
		}
		XMFLOAT3 hw = incoming->GetHalfWidth();
		float a = hw.x * absRotMax.m[i][0];
		float b = hw.y * absRotMax.m[i][1];
		float c = hw.z * absRotMax.m[i][2];
		yourRadius = 
			a + 
			b + 
			c;

		if (abs(temp) > myRadius + yourRadius)
		{
			return 1;
		}
	}

	// L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++)
	{
		// if's to set up x, y, or z components because DirectX won't let me use indexors
		// didn't use XMVectorGetIndex b/c performance drops
		if (i == 0)
		{
			yourRadius = incoming->GetHalfWidth().x;
		}
		else if (i == 1)
		{
			yourRadius = incoming->GetHalfWidth().y;
		}
		else if (i == 2)
		{
			yourRadius = incoming->GetHalfWidth().z;
		}

		myRadius = GetHalfWidth().x * absRotMax.m[0][i] + GetHalfWidth().y * absRotMax.m[1][i] + GetHalfWidth().z * absRotMax.m[2][i];

		if (abs(
			XMVectorGetX(transVec) * rotationMatrix.m[0][i] + 
			XMVectorGetY(transVec) * rotationMatrix.m[1][i] + 
			XMVectorGetZ(transVec) * rotationMatrix.m[2][i]) 
				> myRadius + yourRadius)
		{
			return 1;
		}
	}
	XMFLOAT3 myRad = GetHalfWidth();
	XMFLOAT3 yourRad = incoming->GetHalfWidth();
	 // L = A0 x B0 
	myRadius = GetHalfWidth().y * absRotMax.m[2][0] + GetHalfWidth().z * absRotMax.m[1][0];
	yourRadius = incoming->GetHalfWidth().y * absRotMax.m[0][2] + incoming->GetHalfWidth().z * absRotMax.m[0][1];
	if (abs(XMVectorGetZ(transVec) * rotationMatrix.m[1][0] - XMVectorGetY(transVec) * rotationMatrix.m[2][0]) > myRadius + yourRadius)
	{
		return 1;
	}

	// L = A0 x B1
	myRadius = GetHalfWidth().y * absRotMax.m[2][1] + GetHalfWidth().z * absRotMax.m[1][1];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[0][2] + incoming->GetHalfWidth().z * absRotMax.m[0][0];
	if (abs(XMVectorGetZ(transVec) * rotationMatrix.m[1][1] - XMVectorGetY(transVec) * rotationMatrix.m[2][1]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A0 x B2
	myRadius = GetHalfWidth().y * absRotMax.m[2][2] + GetHalfWidth().z * absRotMax.m[1][2];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[0][1] + incoming->GetHalfWidth().y * absRotMax.m[0][0];
	if (abs(XMVectorGetZ(transVec) * rotationMatrix.m[1][2] - XMVectorGetY(transVec) * rotationMatrix.m[2][2]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A1 x B0
	myRadius = GetHalfWidth().x * absRotMax.m[2][0] + GetHalfWidth().z * absRotMax.m[0][0];
	yourRadius = incoming->GetHalfWidth().y * absRotMax.m[1][2] + incoming->GetHalfWidth().z * absRotMax.m[1][1];
	if (abs(XMVectorGetX(transVec) * rotationMatrix.m[2][0] - XMVectorGetZ(transVec) * rotationMatrix.m[0][0]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A1 x B1
	myRadius = GetHalfWidth().x * absRotMax.m[2][1] + GetHalfWidth().z * absRotMax.m[0][1];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[1][2] + incoming->GetHalfWidth().z * absRotMax.m[1][0];
	if (abs(XMVectorGetX(transVec) * rotationMatrix.m[2][1] - XMVectorGetZ(transVec) * rotationMatrix.m[0][1]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A1 x B2
	myRadius = GetHalfWidth().x * absRotMax.m[2][2] + GetHalfWidth().z * absRotMax.m[0][2];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[1][1] + incoming->GetHalfWidth().y * absRotMax.m[1][0];
	if (abs(XMVectorGetX(transVec) * rotationMatrix.m[2][2] - XMVectorGetZ(transVec) * rotationMatrix.m[0][2]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A2 x B0
	myRadius = GetHalfWidth().x * absRotMax.m[1][0] + GetHalfWidth().y * absRotMax.m[0][0];
	yourRadius = incoming->GetHalfWidth().y * absRotMax.m[2][2] + incoming->GetHalfWidth().z * absRotMax.m[2][1];
	if (abs(XMVectorGetY(transVec) * rotationMatrix.m[0][0] - XMVectorGetX(transVec) * rotationMatrix.m[1][0]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A2 x B1
	myRadius = GetHalfWidth().x * absRotMax.m[1][1] + GetHalfWidth().y * absRotMax.m[0][1];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[2][2] + incoming->GetHalfWidth().z * absRotMax.m[2][0];
	if (abs(XMVectorGetY(transVec) * rotationMatrix.m[0][1] - XMVectorGetX(transVec) * rotationMatrix.m[1][1]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A2 x B2
	myRadius = GetHalfWidth().x * absRotMax.m[1][2] + GetHalfWidth().y * absRotMax.m[0][2];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[2][1] + incoming->GetHalfWidth().y * absRotMax.m[2][0];
	if (abs(XMVectorGetY(transVec) * rotationMatrix.m[0][2] - XMVectorGetX(transVec) * rotationMatrix.m[1][2]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Since no separating axis is found, the OBBs must be intersecting
	return 0;
}

#pragma endregion

#pragma region update
//  ISSUE: WORLD MATRIX DOES NOT UPDATE, MAKING THIS RELATIVE TO CENTER GLOBAAL WOULD BE POGGERS
void  Rigidbody::Update(float deltaTime, float totalTime)  
{
	accel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float tempFric;

	if (!isGrounded)
	{
		// apply gravity
		ApplyGrav(grav);
	}

	// WASD
	ResolveInputs();

	// Applying a normal force from the collision to stop the object
	// TO DO: GET A MORE SOPHISTICATED VERSION OF THIS
	if (IsColliding(EntityManager::GetInstance()->GetRigidBodies()[1]))
	{
		// accel.y = 0.0f;
		// vel.y = 0.0f;
		vel = XMFLOAT3(0.0f, 0.0f, 0.0f);
		accel = XMFLOAT3(0.0f, 0.0f, 0.0f);
		tempFric = EntityManager::GetInstance()->GetRigidBodies()[1]->frictionCoeff;
		tint = XMFLOAT4(1, 0, 0, 0);
	}
	else
	{
		tint = XMFLOAT4(1, 1, 1, 0);
	}

	// add accel to vel
	vel = AddFloat3(vel, accel);

	// apply friction
	if (tempFric > 0.0f)
	{
		ApplyFriction(tempFric);
	}
	else
	{
		ApplyFriction();
	}

	// I don't think I need this because I do the same exact thing in ApplyFriction
	// if vel is tiny, set to 0
	/*if (MagFloat3(vel) < 0.0001f)
	{
		vel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}*/

	// add vel to pos
	pos = AddFloat3(pos, vel);
	myTransform.MoveRelative(vel.x, vel.y, vel.z);

	// updating local to global
	localToGlobalMat = myTransform.GetWorldMatrix();


	// min and max global
	minGlobal = GetMinGlobal();
	maxGlobal = GetMaxGlobal();

}

#pragma endregion


#pragma region Helpers
// helpers

#pragma region XMFLOAT3 Maths
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
	return abs(sqrtf(powf(float3.x, 2.0f) + powf(float3.y, 2.0f) + powf(float3.z, 2.0f)));
}
#pragma endregion

#pragma region ResolveInputs
// gathers inputs and applies the appripriate force
void Rigidbody::ResolveInputs()
{
	// if w, apply force on forward axis
	if (GetAsyncKeyState('W') & 0x8000) 
	{ 
		// use the local z axis
		XMFLOAT3 tempForce = XMFLOAT3(0.0f, 0.0f, 1.0f);

		// multiply the force by the speed
		tempForce = MultFloat3(tempForce, speed);

		//apply the force
		ApplyForce(tempForce); 
	}

	// if S, apply force on (-1)forward axis
	if (GetAsyncKeyState('S') & 0x8000)
	{
		// use the local z axis
		XMFLOAT3 tempForce = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// multiply the force by the speed
		tempForce = MultFloat3(tempForce, speed);

		//apply the force
		ApplyForce(tempForce);
	}

	// if a, negative rotation on the Y axis
	if (GetAsyncKeyState('A') & 0x8000)
	{
		// negative rotation on Y axis by the turn radius
		myTransform.Rotate(0.0f, -turnRadius, 0.0f);
	}

	// if d, positive rotation on the Y axis
	if (GetAsyncKeyState('D') & 0x8000)
	{
		// positive rotation on Y axis by the turn radius
		myTransform.Rotate(0.0f, turnRadius, 0.0f);
	}
}
#pragma endregion

#pragma endregion