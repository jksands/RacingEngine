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
	// movement stuff
	DirectX::XMFLOAT3 accel = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR rotQuat;
	DirectX::XMFLOAT3 size = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT3 vel = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	float mass = 1.0f;

	// gravity
	float grav = 4.0f;
	float speed = 7.5f;
	float turnRadius = 0.5f;
	float frictionCoeff = 0.0f;

	// Is it dynamic?
	bool isDynamic;

	// Represent the offset from our rigidbody's global center to our parent's global center
	DirectX::XMFLOAT3 offset;

	// Represents the vector offset to point the velocity to while STEERING
	DirectX::XMFLOAT3 steeringOffset;

	// this... thing?
	const float EPSILON = 1.401298E-45;

	bool movingBackward;

	// public methods
public:
	// transform info
	Transform* myTransform;
	DirectX::XMFLOAT4 tint = DirectX::XMFLOAT4(1, 1, 1, 0);
	/*
	* Constructor
	* Params:
	*	std::vector<DirectX::XMFLOAT3> pointList: the points to make the rigidbody out of
	* Returns: an instance of the class
	*/
	Rigidbody(std::vector<Vertex> vertices, Transform* incomingTransform, bool _isDynamic = true, float friction = 0.0f);
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
	void ApplyFriction(float incomingFrictionCoefficient = 0.005f);
	/*
	* Applies a force to the rigidbody
	* ARGUMENTS: DirectX::XMFLOAT3 incomingForce -- force being applied to the rigidbody
	* OUTPUT: NONE
	*/
	void ApplyForce(DirectX::XMFLOAT3 incomingForce);

	void ApplyGrav(float gravity);

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
	* Uses ARBB collision detections to return if this is colliding with another giidbody
	* params: incoing rigidbody
	* returns true if colliding false if not collidiing
	*/
	bool ARBBCheck(Rigidbody* incoming);

	// handles driving with int telling us whic direction the car should be going (which way the force is going)
	void HandleDrive(int dir);

	// Handles steering with an int telling us whether we're steering right or left
	void HandleSteering(int dir, float dt);

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
