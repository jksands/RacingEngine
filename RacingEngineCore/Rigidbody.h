#pragma once
#include "Vertex.h"
#include "Transform.h"
#include <DirectXMath.h>
#include <vector>
class Rigidbody
{
public:
	typedef Rigidbody* RigidbodyPtr;
	// protected fields
protected:
	// bools to tell us if the collisions boxes are visible
	bool boundingSphereVisible = false;
	bool OBBVisible = false;
	bool ARBBVisible = true;
	float radius = 0.0f;

	bool isGrounded = false;

	// colour of the bounding boxes when they're drawn
	DirectX::XMFLOAT3 collidingColour = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 notCollidingColour = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// local space variables
	DirectX::XMFLOAT3 centerLocal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 minLocal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 maxLocal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// global variables
	DirectX::XMFLOAT3 centerGlobal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 minGlobal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 maxGlobal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// half-widthfir Ibb 
	DirectX::XMFLOAT3 halfWidthOBB = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// size variable for ARBB
	DirectX::XMFLOAT3 sizeARBB = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	// local to world space matrix
	DirectX::XMFLOAT4X4 localToGlobalMat;
	// XMStoreFloat4x4(&localToGlobalMat, XMMatrixIdentity());
			// colliding array info
	int collidingCount = 0;
	Rigidbody* collidingArray = nullptr;
	// transform info
	Transform myTransform;
	// movement stuff
	DirectX::XMFLOAT3 accel = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR rotQuat;
	DirectX::XMFLOAT3 size = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT3 vel = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float mass = 1.0f;

	// gravity
	float grav = 0.011f;
	// Is it dynamic?
	bool isDynamic;

	// Represent the offset from our rigidbody's global center to our parent's global center
	DirectX::XMFLOAT3 offset;

	// this... thing?
	const float EPSILON = 1.401298E-45;

	// public methods
public:
	/*
	* Constructor
	* Params:
	*	std::vector<DirectX::XMFLOAT3> pointList: the points to make the rigidbody out of
	* Returns: an instance of the class
	*/
	Rigidbody(std::vector<Vertex> vertices, Transform incomingTransform, bool _isDynamic = true);
	/*
	* Copy Constructor
	* Params:
	*	Rigidbody const& incoming: incoming instance to copy
	* Returns: an instance of the class
	*/
	Rigidbody(Rigidbody const& incoming);
	/*
	* Copy Assignment Operator
	* Params:
	*	Rigidbody const& incoming: incoming instance to copy
	* Returns: an instance of the class
	*/
	Rigidbody& operator=(Rigidbody& incoming);


	/*
* Destructor
* Params: N/A
* Returns: N/A
*/
	~Rigidbody();
	/*
	* Clear the colliding list
	* Params:
	*	None
	* Returns: None
	*/
	void ClearCollisionList();
	// TO DO: ADD AND REMOVE FROM COLLISION LIST IS A LATER ISSUE
	/*
	* Adding collision with the colliding rigidbody
	* Params:
	*	Rigidbody* incoming -- rigidbody I'm colliding with
	* Returns: None
	*/
	void AddToCollisionList(Rigidbody* incoming);
	/*
	* Removers a collision with the incoming rigidbody
	* Params:
	*	Rigidbody* incoming
	* Returns: None
	*/
	void RemoveCollisionList(Rigidbody* incoming);
	/*
	* Checks to see if this rigidbody is colliding with the incoing rigidbody
	* Params:
	*	Rigidbody* incoming -- rigidbody to check collisions against
	* Returns: boolean telling us if the two are colliding
	*/
	bool IsColliding(Rigidbody* incoming);

	/*
	* multiplies vel by friction coefficient
	* ARGUMENTS:
	*	float incomingFrictionCoefficient -- coefficient of friction to be used in the friction equations
	OUTPUT: NONE
	*/
	void ApplyFriction(float incomingFrictionCoefficient = 0.05f);
	/*
	* Applies a force to the rigidbody
	* ARGUMENTS: DirectX::XMFLOAT3 incomingForce -- force being applied to the rigidbody
	* OUTPUT: NONE
	*/
	void ApplyForce(DirectX::XMFLOAT3 incomingForce);
	// TO DO: MILESTONE 2
	/*
	* does the SAT collision check
	* Arguments: the incoming rigidbody to collide with
	* Returns: 1 if no collision 0 if collision
	*/
	int SAT(Rigidbody* incoming);
	/*
	* Updates the rigidbody
	* ARGUMENTS: NONE
	* OUTPUT: NONE
	*/
	void Update(float deltaTime, float totalTime);
	/*
	* adds the two DirectX::XMFLOAT3 components and return an DirectX::XMFLOAT3
	* Arguments:
	*	two DirectX::XMFLOAT3's to add
	* Returns: added DirectX::XMFLOAT3
	*/
	DirectX::XMFLOAT3 AddFloat3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

	/*
	* subtracts the two DirectX::XMFLOAT3 components and return an DirectX::XMFLOAT3
	* Arguments:
	*	two DirectX::XMFLOAT3's to subtract, in the order the would be subtracted (a - b)
	* Returns: subtracted DirectX::XMFLOAT3
	*/
	DirectX::XMFLOAT3 SubFloat3(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);
	/*
	* divides an DirectX::XMFLOAT3 by a scalar
	* Arguments:
	*	the DirectX::XMFLOAT3 to divide and the float scalar to divide the DirectX::XMFLOAT3 by
	* Returns: divided DirectX::XMFLOAT3
	*/
	DirectX::XMFLOAT3 DivFloat3(DirectX::XMFLOAT3 float3, float scalar);
	/*
	* multiplies an DirectX::XMFLOAT3 by a scalar
	* Arguments:
	*	the DirectX::XMFLOAT3 to multiply and the float scalar to multiply the DirectX::XMFLOAT3 by
	* Returns: multiplied  DirectX::XMFLOAT3
	*/
	DirectX::XMFLOAT3 MultFloat3(DirectX::XMFLOAT3 float3, float scalar);
	/*
	* finds the magnitude of a DirectX::XMFLOAT3
	* Arguments:
	*	DirectX::XMFLOAT3 to find the magnitude of
	* Returns: magnitude of DirectX::XMFLOAT3
	*/
	float MagFloat3(DirectX::XMFLOAT3 float3);

	/*
	* Uses ARBB collision detections to return if this is colliding with another giidbody
	* params: incoing rigidbody
	* returns true if colliding false if not collidiing
	*/
	bool ARBBCheck(Rigidbody* incoming);
#pragma region GettersAndSetters
	// bounding spehere visibility
	bool GetBoundingSphereVisible();
	void SetBoundingSphereVisible(bool value);
	// Object Oriented Bounding Box visibility
	bool GetOBBVisible();
	void SetOBBVisible(bool value);
	// Axis-Realigned Bounding Box visibility
	bool GetARBBVisible();
	void SetARBBVisible(bool value);
	// returns radius
	float GetRadius();
	// colliding colour
	DirectX::XMFLOAT3 GetCollidingColour();
	void SetCollidingColour(DirectX::XMFLOAT3 value);
	// not colliding colour
	DirectX::XMFLOAT3 GetNotCollidingCOlour();
	void SetNotCollidingColour(DirectX::XMFLOAT3 value);
	// local variables
	DirectX::XMFLOAT3 GetCentreLocal();
	DirectX::XMFLOAT3 GetMinLocal();
	DirectX::XMFLOAT3 GetMaxLocal();

	//global variables
	DirectX::XMFLOAT3 GetCenterGlobal();
	DirectX::XMFLOAT3 GetMinGlobal();
	DirectX::XMFLOAT3 GetMaxGlobal();
	// halfWidth 
	DirectX::XMFLOAT3 GetHalfWidth();

	// matrix model
	DirectX::XMFLOAT4X4 GetModelMatrix();
	void SetModelMatrix(DirectX::XMFLOAT4X4 ModelMatrix);
	// collision array stuff
	Rigidbody* GetCollidingArray();
	int GetCollidingCount();
	// position getter and setter
	void SetPosition(DirectX::XMFLOAT3 incomingPos);
	DirectX::XMFLOAT3 GetPosition();
	// rotation getter and setter
	void SetRotation(DirectX::XMVECTOR incomingRot);
	DirectX::XMVECTOR GetRotation();
	// size getter and setter
	void SetSize(DirectX::XMFLOAT3 incomingSize);
	DirectX::XMFLOAT3 GetSize();
	// velocity getter and setter
	void SetVelocity(DirectX::XMFLOAT3 incomingVel);
	DirectX::XMFLOAT3 GetVelocity();
	// mass getter and setter
	void SetMass(float incomingMass);
	float GetMass();

	DirectX::XMFLOAT3 GetParentalOffset();
#pragma endregion
};
