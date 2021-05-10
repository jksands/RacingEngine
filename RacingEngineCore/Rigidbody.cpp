#include "Rigidbody.h"
#include "Transform.h"
#include "EntityManager.h"
#include "Helpers.h"
using namespace DirectX;
// constructor & big 3

#pragma region Big3
/*
* Constructor
* Params:
*	std::vector<XMFLOAT3> pointList: the points to make the rigidbody out of
* Returns: an instance of the class
*/
Rigidbody::Rigidbody(std::vector<Vertex> vertices, Transform* incomingTransform, bool _isDynamic, float friction)
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
	localToGlobalMat = myTransform->GetWorldMatrix();

	// half width is dist between min and max over 2
	halfWidthOBB = DivFloat3(SubFloat3(maxLocal, minLocal), 2.0f);
	XMFLOAT3 tempScale = myTransform->GetScale();
	halfWidthOBB = XMFLOAT3(halfWidthOBB.x * tempScale.x, halfWidthOBB.y * tempScale.y, halfWidthOBB.z * tempScale.z);
	radius = MagFloat3(halfWidthOBB);

	// setting max global and min global
	// min
	minGlobal = GetMinGlobal();

	// max
	maxGlobal = GetMaxGlobal();
	// maxGlobal = MultFloat3(maxGlobal, incomingTransform.GetScale().x);

	// find the centre
	centerLocal = DivFloat3(AddFloat3(maxLocal, minLocal), 2.0f);
	// find the radius by distance bettwen centre and either min or max
	// radius = MagFloat3(SubFloat3(maxGlobal, GetCenterGlobal()));
	rotQuat = XMQuaternionIdentity();
	ARBBVisible = true;
	vel = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// calculate center global
	XMMATRIX mat = XMLoadFloat4x4(&localToGlobalMat);
	XMVECTOR pos = XMLoadFloat3(&centerLocal);
	XMStoreFloat3(&centerGlobal, XMVector3Transform(pos, mat));
	// centerGlobal = GetCenterGlobal();

	// After calculating center global, get the offset between US and the parent center
	offset = SubFloat3(myTransform->GetPosition(), centerGlobal);

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
		// isGrounded = true;
	}
	else
	{
		isColliding = false;
		// isGrounded = false;
	}

	// run SAT now
	if (isColliding)
	{
		if (SAT(incoming) == 1)
		// if (ARBBCheck(incoming) == false)
		{
			isColliding = false;
			// isGrounded = false;
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
	if (incomingFrictionCoefficient < 0.000001f)
	{
		incomingFrictionCoefficient = 0.000001f;
	}

	// coudl probs change these to just the x and z components. Don't need to update the y
	// vel = MultFloat3(vel, 1.0f - incomingFrictionCoefficient);
	vel.x *= (1 - incomingFrictionCoefficient);
	vel.z *= (1 - incomingFrictionCoefficient);

	if (MagFloat3(vel) < 0.001f)
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

	XMFLOAT3 xAxisFloat = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 yAxisFloat = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 zAxisFloat = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMVECTOR xAxis = XMLoadFloat3(&xAxisFloat);
	XMVECTOR yAxis = XMLoadFloat3(&yAxisFloat);
	XMVECTOR zAxis = XMLoadFloat3(&zAxisFloat);
	
	// myTransform->Rotate(0.0f, 10.0f, 0.0f);
	XMMATRIX myWorldMat = XMLoadFloat4x4(&myTransform->GetWorldMatrix());
	XMMATRIX yourWorldMat = XMLoadFloat4x4(&incoming->myTransform->GetWorldMatrix());
	// XMStoreFloat3(&centerGlobal, XMVector3Transform(p, mat));

	XMFLOAT3 myR = myTransform->GetPitchYawRoll();
	XMVECTOR aQuat = XMQuaternionRotationRollPitchYaw(myR.x, myR.y, myR.z);
	// get all the axes for each object
	XMVECTOR myAxes[] = {
		XMVector3Normalize(XMVector3Rotate(xAxis, aQuat)),
		XMVector3Normalize(XMVector3Rotate(yAxis, aQuat)),
		XMVector3Normalize(XMVector3Rotate(zAxis, aQuat))
		// XMVector3Normalize(XMVector3Transform(xAxis, myWorldMat)),
		// XMVector3Normalize(XMVector3Transform(yAxis, myWorldMat)),
		// XMVector3Normalize(XMVector3Transform(zAxis, myWorldMat))
		// XMVector3Transform(xAxis, myWorldMat),
		// XMVector3Transform(yAxis, myWorldMat),
		// XMVector3Transform(zAxis, myWorldMat)
	};
	XMFLOAT3 rotation = incoming->myTransform->GetPitchYawRoll();
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMVECTOR yourAxes[] = {
		XMVector3Normalize(XMVector3Rotate(xAxis, quat)),
		XMVector3Normalize(XMVector3Rotate(yAxis, quat)),
		XMVector3Normalize(XMVector3Rotate(zAxis, quat))
		// XMVector3Normalize(XMVector3Transform(xAxis, yourWorldMat)),
		// XMVector3Normalize(XMVector3Transform(yAxis, yourWorldMat)),
		// XMVector3Normalize(XMVector3Transform(zAxis, yourWorldMat))
		// XMVector3Transform(xAxis, yourWorldMat),
		// XMVector3Transform(yAxis, yourWorldMat),
		// XMVector3Transform(zAxis, yourWorldMat)
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


	XMFLOAT3 myGlob = GetCenterGlobal();
	XMFLOAT3 yourGlob = incoming->GetCenterGlobal();
	// compute the translation vector
	// incoming centre global - my center global
	XMVECTOR transVec = XMLoadFloat3(&SubFloat3(yourGlob, myGlob));

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

	XMFLOAT3 hw = incoming->GetHalfWidth();
	XMFLOAT3 rotVec;
	XMStoreFloat3(&rotVec, XMVector3Rotate(XMLoadFloat3(&hw), aQuat));
	// L = A0, l + A1, L = A2
	for (int i = 0; i < 3; i++)
	{
		float temp;
		// if's to set up x, y, or z components because DirectX won't let me use indexors
		// didn't use XMVectorGetIndex b/c performance drops
		if (i == 0)
		{
			myRadius = GetHalfWidth().x;
			yourRadius = rotVec.x;
			temp = XMVectorGetX(transVec);
		}
		else if (i == 1)
		{
			myRadius = GetHalfWidth().y;
			yourRadius = rotVec.y;
			temp = XMVectorGetY(transVec);
		}
		else if (i == 2)
		{
			myRadius = GetHalfWidth().z;
			yourRadius = rotVec.z;
			temp = XMVectorGetZ(transVec);
		}
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

	hw = GetHalfWidth();
	XMStoreFloat3(&rotVec, XMVector3Rotate(XMLoadFloat3(&hw), quat));

	// L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++)
	{
		float temp;
		// if's to set up x, y, or z components because DirectX won't let me use indexors
		// didn't use XMVectorGetIndex b/c performance drops
		if (i == 0)
		{
			yourRadius = incoming->GetHalfWidth().x;
			myRadius = rotVec.x;
			temp = XMVectorGetX(transVec);
			
		}
		else if (i == 1)
		{
			yourRadius = incoming->GetHalfWidth().y;
			myRadius = rotVec.y;
			temp = XMVectorGetY(transVec);
		}
		else if (i == 2)
		{
			yourRadius = incoming->GetHalfWidth().z;
			myRadius = rotVec.z;
			temp = XMVectorGetZ(transVec);
		}

		myRadius = GetHalfWidth().x * absRotMax.m[0][i] + GetHalfWidth().y * absRotMax.m[1][i] + GetHalfWidth().z * absRotMax.m[2][i];

		 if (abs(
		 	XMVectorGetX(transVec) * rotationMatrix.m[0][i] + 
		 	XMVectorGetY(transVec) * rotationMatrix.m[1][i] + 
		 	XMVectorGetZ(transVec) * rotationMatrix.m[2][i]) 
		 		> myRadius + yourRadius)
		// if (abs(temp) > myRadius + yourRadius)
		{
			return 1;
		}
	}
	XMFLOAT3 myRad = GetHalfWidth();
	XMFLOAT3 yourRad = incoming->GetHalfWidth();
	XMFLOAT3 tvf;

	XMStoreFloat3(&tvf, transVec);
	 // L = A0 x B0 - YES
	myRadius = GetHalfWidth().y * absRotMax.m[2][0] + GetHalfWidth().z * absRotMax.m[1][0];
	yourRadius = incoming->GetHalfWidth().y * absRotMax.m[0][2] + incoming->GetHalfWidth().z * absRotMax.m[0][1];
	if (abs(tvf.z * rotationMatrix.m[1][0] - tvf.y * rotationMatrix.m[2][0]) > myRadius + yourRadius)
	{
		return 1;
	}

	// L = A0 x B1 - YES
	myRadius = GetHalfWidth().y * absRotMax.m[2][1] + GetHalfWidth().z * absRotMax.m[1][1];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[0][2] + incoming->GetHalfWidth().z * absRotMax.m[0][0];
	if (abs(tvf.z * rotationMatrix.m[1][1] - tvf.y * rotationMatrix.m[2][1]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A0 x B2 - YES
	myRadius = GetHalfWidth().y * absRotMax.m[2][2] + GetHalfWidth().z * absRotMax.m[1][2];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[0][1] + incoming->GetHalfWidth().y * absRotMax.m[0][0];
	if (abs(tvf.z * rotationMatrix.m[1][2] - tvf.y * rotationMatrix.m[2][2]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A1 x B0 - YES
	myRadius = GetHalfWidth().x * absRotMax.m[2][0] + GetHalfWidth().z * absRotMax.m[0][0];
	yourRadius = incoming->GetHalfWidth().y * absRotMax.m[1][2] + incoming->GetHalfWidth().z * absRotMax.m[1][1];
	if (abs(tvf.x * rotationMatrix.m[2][0] - tvf.z * rotationMatrix.m[0][0]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A1 x B1 - YES
	myRadius = GetHalfWidth().x * absRotMax.m[2][1] + GetHalfWidth().z * absRotMax.m[0][1];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[1][2] + incoming->GetHalfWidth().z * absRotMax.m[1][0];
	if (abs(tvf.x * rotationMatrix.m[2][1] - tvf.z * rotationMatrix.m[0][1]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A1 x B2 - YES
	myRadius = GetHalfWidth().x * absRotMax.m[2][2] + GetHalfWidth().z * absRotMax.m[0][2];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[1][1] + incoming->GetHalfWidth().y * absRotMax.m[1][0];
	if (abs(tvf.x * rotationMatrix.m[2][2] - tvf.z * rotationMatrix.m[0][2]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A2 x B0 - YES
	myRadius = GetHalfWidth().x * absRotMax.m[1][0] + GetHalfWidth().y * absRotMax.m[0][0];
	yourRadius = incoming->GetHalfWidth().y * absRotMax.m[2][2] + incoming->GetHalfWidth().z * absRotMax.m[2][1];
	if (abs(tvf.y * rotationMatrix.m[0][0] - tvf.x * rotationMatrix.m[1][0]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Test axis L = A2 x B1 - YES
	myRadius = GetHalfWidth().x * absRotMax.m[1][1] + GetHalfWidth().y * absRotMax.m[0][1];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[2][2] + incoming->GetHalfWidth().z * absRotMax.m[2][0];
	if (abs(tvf.y * rotationMatrix.m[0][1] - tvf.x * rotationMatrix.m[1][1]) > myRadius + yourRadius)
	{
		return 1;
	}
	
	// Test axis L = A2 x B2
	myRadius = GetHalfWidth().x * absRotMax.m[1][2] + GetHalfWidth().y * absRotMax.m[0][2];
	yourRadius = incoming->GetHalfWidth().x * absRotMax.m[2][1] + incoming->GetHalfWidth().y * absRotMax.m[2][0];
	if (abs(tvf.y * rotationMatrix.m[0][2] - tvf.x * rotationMatrix.m[1][2]) > myRadius + yourRadius)
	{
		return 1;
	}

	// Since no separating axis is found, the OBBs must be intersecting
	return 0;
}

#pragma endregion

#pragma region update
// TODO: make speed relative to a function of vel
//  -- make a button for cruise control
//  -- make a button for just brakes
void  Rigidbody::Update(float deltaTime, float totalTime)  
{
	float tempFric = 0.0f;

	if (!isGrounded)
	{
		// apply gravity
		ApplyGrav(grav);
	}

	// add accel to vel
	vel = AddFloat3(vel, MultFloat3(accel, deltaTime));
	// vel.x = 4.5f;
	// vel.z = 4.5f;
	XMFLOAT3 tempVel = vel;
	// add steering to vel
	if (MagFloat3(steeringOffset) > 0)
	{
		float velMag = MagFloat3(vel);
		vel = AddFloat3(vel, steeringOffset);
		XMStoreFloat3(&vel, XMVector3Normalize(XMLoadFloat3(&vel)) * velMag);

	}

	// vel = MultFloat3(vel, deltaTime);

	// only apply friction is we are not pressing a key
	if (!isDrive)
	{
		// apply friction
		if (tempFric > 0.0f)
		{
			ApplyFriction(tempFric);
		}
		else
		{
			ApplyFriction();
		}
	}

	// checking for max speed
	if (MagFloat3(vel) > maxSpeed)
	{
		XMFLOAT3 t;
		XMStoreFloat3(&t, XMVector3Normalize(XMLoadFloat3(&vel)));
		vel = MultFloat3(t, maxSpeed);
	}

	// add vel to pos
	pos = AddFloat3(pos, MultFloat3(vel, deltaTime));
	// Rotate velocity to global
	XMVECTOR tVel = XMLoadFloat3(&vel);

	// rotate the vel using car's world mat
	XMFLOAT3 myR = myTransform->GetPitchYawRoll();
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYaw(myR.x, myR.y, myR.z);
	tVel = XMVector3Rotate(tVel, rotQuat);
	// Relative move
	// myTransform->MoveRelative(vel.x * deltaTime, vel.y * deltaTime, vel.z * deltaTime);
	XMFLOAT3 v;
	XMStoreFloat3(&v, tVel);
	// Global move
	myTransform->MoveAbsolute(v.x * deltaTime, v.y * deltaTime, v.z * deltaTime);
	// negate gravity (don't point downward)
	tempVel.y = 0;
	if (MagFloat3(tempVel) == 0)
	{
		tempVel = XMFLOAT3(0, 0, 1);
		
	}
	// Always look forward
	tempVel.x = abs(tempVel.x);
	tempVel.z = abs(tempVel.z);

	// tempVel.x = -1;
	// tempVel.z = -1;

	// Get our rotation and make a quat out of it
	//XMFLOAT3 myR = myTransform->GetPitchYawRoll();
	//// NOTE: SHOULD REDUCE THIS TO ONLY HAPPEN WHEN DIRTY
	//XMVECTOR rotQuat = XMQuaternionRotationRollPitchYaw(myR.x, myR.y, myR.z);
	XMStoreFloat3(&tempVel, XMVector3Rotate(XMVector3Normalize(XMLoadFloat3(&tempVel)), rotQuat));
	// myTransform->LookAt(XMLoadFloat3(&GetCenterGlobal()), XMLoadFloat3(&tempVel));


	// updating local to global
	localToGlobalMat = myTransform->GetWorldMatrix();


	// min and max global
	minGlobal = GetMinGlobal();
	maxGlobal = GetMaxGlobal();

	// Reset accel
	accel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	// Reset steering offset
	steeringOffset = XMFLOAT3(0, 0, 0);
	// reset is drive
	isDrive = false;
	// Check collisions
	AddToCollisionList(deltaTime);
}
void Rigidbody::AddToCollisionList(float dt)
{
	collisionList.clear();
	std::vector<Rigidbody*> rbs = EntityManager::GetInstance()->GetRigidBodies();
	// loop through all entities in entity Manager
	for (int i = 1; i < rbs.size(); i++)
	{
		if (IsColliding(rbs[i]))
		{
			collisionList.push_back(rbs[i]);
		}
	}
	ResolveCollisions(dt);
	
}
void Rigidbody::ResolveCollisions(float dt)
{
	tint = XMFLOAT4(1, 1, 1, 0);
	for (int i = 0; i < collisionList.size(); i++)
	{
		XMFLOAT3 me = GetCenterGlobal();
		XMFLOAT3 you = collisionList[i]->GetCenterGlobal();
		// check for ground collision


		if (you.y < me.y) // you are beneath me
		{
			// colliding with ground
			// THIS WORKS AND KEEPS CAR STILL
			accel.y = 0.0f;
			vel.y = 0.0f;
			// vel = XMFLOAT3(0.0f, 0.0f, 0.0f);
			// accel = XMFLOAT3(0.0f, 0.0f, 0.0f);
			// tempFric = EntityManager::GetInstance()->GetRigidBodies()[1]->frictionCoeff;
			tint = XMFLOAT4(1, 0, 0, 0);
			// Set to be slightly above ground (will do a projected collision check later to check
			// if should fall)
			myTransform->MoveRelative(0, .1f, 0);
			isGrounded = true;
		}
		else
		{

			// get reference to the thing we are colliding with
			Rigidbody* inc = collisionList[i];
			XMFLOAT3 distVec = SubFloat3(you, me);

			XMFLOAT3 incScale = inc->myTransform->GetScale();
			// dividing by scale to treat like cube
			distVec.x /= incScale.x/2.0f;
			distVec.z /= incScale.z/2.0f;

			// need this to reflect across later
			XMVECTOR reflectNorm;

			XMVECTOR tempVel = XMLoadFloat3(&vel);

			// rotate the vel using car's world mat
			XMFLOAT3 myR = myTransform->GetPitchYawRoll();
			XMVECTOR rotQuat = XMQuaternionRotationRollPitchYaw(myR.x, myR.y, myR.z);
			tempVel = XMVector3Rotate(tempVel, rotQuat);

			// left/right 
			if (abs(distVec.x) > abs(distVec.z))
			{
				// tempVel = -tempVel;
				// to the right
				if (distVec.x > 0)
				{
					// reflect norm is the right vector
					reflectNorm = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
				}
				// to the left
				else
				{
					// reflect norm is the -right vector
					reflectNorm = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f); // DOES NOT WORK
					// reflectNorm = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
				}
			}
			// front or back
			else if (abs(distVec.x) < abs(distVec.z))
			{
				// to the front
				if (distVec.z > 0)
				{
					// reflect norm is the forward vector
					reflectNorm = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
					// reflectNorm = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
				}
				// to the back
				else
				{
					// reflect norm is the -forward vector
					reflectNorm = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
				}
			}
			// if they are the same, it hit the corner, just reverse vel
			else
			{
				reflectNorm = XMVectorSet(-vel.x, 0.0f, -vel.z, 0.0f);
			}

			// relfecting it
			tempVel = XMVector3Reflect(tempVel, reflectNorm);
			// myTransform->LookAt(XMLoadFloat3(&pos), -tempVel);
			// "un-rotate" the reflected velocity so it goes back to local space
			rotQuat = XMQuaternionInverse(rotQuat);
			tempVel = XMVector3Rotate(tempVel, rotQuat);
			// myTransform->SetPosition(pos.x + offset.x, pos.y + offset.y, pos.z + offset.z);

			// re storing the velocity
			XMStoreFloat3(&vel, tempVel);

			// Setting tint
			tint = XMFLOAT4(1, 0, 0, 0);

			// moving it slightly away from the wall so it's not stuck inside
			// myTransform->MoveRelative(vel.x * dt, vel.y * dt, vel.z * dt);
		}
	}
}
#pragma endregion

#pragma region Drive

void Rigidbody::HandleDrive(int dir)
{
	isDrive = true;
	if (dir == 1)
	{
		// use the local z axis
		XMFLOAT3 tempForce = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// multiply the force by the speed
		tempForce = MultFloat3(tempForce, speed);

		// apply the force
		ApplyForce(tempForce);
	}
	else if (dir == -1)
	{
		// use the local z axis
		XMFLOAT3 tempForce = XMFLOAT3(0.0f, 0.0f, 1.0f);

		// multiply the force by the speed
		tempForce = MultFloat3(tempForce, speed);

		//apply the force
		ApplyForce(tempForce);
	}
}

#pragma endregion

#pragma region Steering

void Rigidbody::HandleSteering(int dir, float dt)
{
	// Can't steer if not moving
	if (vel.x == 0 && vel.z == 0)
		return;
	// Stores velocity temporarily 
	XMFLOAT3 tempVel = vel;
	// Get the normalized vel for dot product
	XMVECTOR normalVel = XMVector3Normalize(XMLoadFloat3(&vel));
	// Get the normalized forward
	XMVECTOR normalFor = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); // -1 here since the car model is flipped
	// Get our rotation and make a quat out of it
	XMFLOAT3 myR = myTransform->GetPitchYawRoll();
	// NOTE: SHOULD REDUCE THIS TO ONLY HAPPEN WHEN DIRTY
	XMVECTOR rotQuat = XMQuaternionRotationRollPitchYaw(myR.x, myR.y, myR.z);
	normalVel = XMVector3Rotate(normalVel, rotQuat);
	// rotate local forward to be car's forward
	normalFor = XMVector3Rotate(normalFor, rotQuat);

	// store the result -- will be 1 if pointing in same direction (moving forward)
	float dotRes = XMVectorGetX(XMVector3Dot(normalVel, normalFor));
	dotRes = dotRes < 0 ? -1 : 1;
	// steer right
	if (dir == 1)
	{
		// Need to set the offset vector here --
		// Start with our local right
		XMFLOAT3 localRight = XMFLOAT3(-1.1f * dotRes, 0, 0);
		// ApplyForce(DivFloat3(localRight, 10));
		// return;
		XMVECTOR vecRight = XMLoadFloat3(&localRight);
		// vecRight = XMVector3Rotate(vecRight,rotQuat);

		// Apply that to our forward in drive
		steeringOffset = localRight;
		XMFLOAT3 newVel;
		// Add the right vector to the normalized velocity vector
		XMVECTOR newVelVec = vecRight + normalVel;
		newVelVec = XMVector3Normalize(newVelVec);
		float temp = MagFloat3(vel);
		newVelVec = newVelVec * temp;
		

		// Store result into newVel float3
		XMStoreFloat3(&newVel, newVelVec);
		// newVel.y = 0;
		// Normalize it by converting it to a vector and back to a float
		// XMStoreFloat3(&newVel, XMVector3Normalize(XMLoadFloat3(&newVel)));
		// Make the new Vel have the same magnitude as the old velocity
		newVel.y = vel.y;
		// CALC ANGLE OF ROTATION
		XMVECTOR posVec = XMLoadFloat3(&pos);
		XMVECTOR a = normalVel;
		XMVECTOR b = XMVector3Normalize(newVelVec);
		// XMMATRIX tempMat = XMMatrixLookToLH(posVec, b, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		
		XMVECTOR result = XMVector3AngleBetweenNormals(a, b);
		float angle = XMVectorGetX(result);

		// CHange the velocity -- THIS UPDATES THE VELOCTY
		// vel = newVel;
		// float rotateAmt = -1 * localRight.x * angle * dt;
		// myTransform->Rotate(0, rotateAmt, 0);
		// myTransform->LookAt(posVec, b);
	}
	// steer left
	else if (dir == -1)
	{
		// Need to set the offset vector here --
		// Start with our local left
		XMVECTOR globalLeft = XMVectorSet(.1f * dotRes, 0, 0, 0);
		// Apply car's transformation to local left
		globalLeft = XMVector3Rotate(globalLeft, rotQuat);

		// XMStoreFloat3(&steeringOffset, globalLeft);

		// XMFLOAT3 t;
		// XMStoreFloat3(&t, globalLeft);
		// t = MultFloat3(t, speed);
		// ApplyForce(t);
		// return;


		// Get the global representation of our velocity
		XMVECTOR globalVel = XMLoadFloat3(&vel);
		// Rotate it
		globalVel = XMVector3Rotate(globalVel, rotQuat);

		// Now that we have both components in world space, add them together to get our mutated velocity
		XMVECTOR velFinal = globalVel + globalLeft;
		// Normalize vector, then multiply by initial magnitude
		velFinal = XMVector3Normalize(velFinal) * MagFloat3(vel);
		// Finally, rotate it back to local by rotating by inverse RotQuat
		velFinal = XMVector3Rotate(velFinal, XMQuaternionInverse(rotQuat));
		myTransform->LookAt(XMLoadFloat3(&GetCenterGlobal()), velFinal);
		

		// Now just store it back into vel
		XMStoreFloat3(&vel, velFinal);
		// ApplyForce(localRight);
		// pass the right vector through the rotation matrix...
		// Get our rotation and make a quat out of it
		// XMFLOAT3 myR = myTransform->GetPitchYawRoll();
		// // NOTE: SHOULD REDUCE THIS TO ONLY HAPPEN WHEN DIRTY
		// XMVECTOR rotQuat = XMQuaternionRotationRollPitchYaw(myR.x, myR.y, myR.z);
		// // Rotate the right and store it back into our local right
		// XMStoreFloat3(&localRight, XMVector3Rotate(XMLoadFloat3(&localRight), rotQuat));
		// Apply that to our forward in drive
		// steeringOffset = localLeft;

		// Get where our new velocity should point
		//XMFLOAT3 newVel = // SubFloat3(
		//	AddFloat3(vel, localLeft)
		//	// , pos)
		//	;
		// newVel.y = 0;
		// Normalize it by converting it to a vector and back to a float
		//XMStoreFloat3(&newVel, XMVector3Normalize(XMLoadFloat3(&newVel)));
		//// Make the new Vel have the same magnitude as the old velocity
		//float temp = MagFloat3(vel);
		//newVel = MultFloat3(newVel, temp);
		//newVel.y = vel.y;
		//// CALC ANGLE OF ROTATION
		//XMVECTOR a = XMLoadFloat3(&vel);
		//XMVECTOR b = XMLoadFloat3(&newVel);
		//XMVECTOR result = XMVector3AngleBetweenVectors(a, b);
		//float angle = XMVectorGetX(result);

		//// CHange the velocity
		//vel = newVel;

		// myTransform->Rotate(0, -1 * localLeft.x * angle * dt, 0);
	}
}

#pragma endregion