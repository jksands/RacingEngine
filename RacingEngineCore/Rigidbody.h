#pragma once
#include "Vertex.h"
#include "Transform.h"
namespace DirectX {
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
		// colour of the bounding boxes when they're drawn
		XMFLOAT3 collidingColour = XMFLOAT3(1.0f, 0.0f, 0.0f);
		XMFLOAT3 notCollidingColour = XMFLOAT3(0.0f, 0.0f, 0.0f);
		// local space variables
		XMFLOAT3 centerLocal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 minLocal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 maxLocal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		// global variables
		XMFLOAT3 centerGlobal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 minGlobal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 maxGlobal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		// half-widthfir Ibb 
		XMFLOAT3 halfWidthOBB = XMFLOAT3(0.0f, 0.0f, 0.0f);
		// size variable for ARBB
		XMFLOAT3 sizeARBB = XMFLOAT3(0.0f, 0.0f, 0.0f);
		// local to world space matrix
		XMFLOAT4X4 localToGlobalMat;
		// XMStoreFloat4x4(&localToGlobalMat, XMMatrixIdentity());
				// colliding array info
		int collidingCount = 0;
		Rigidbody* collidingArray = nullptr;
		// transform info
		Transform myTransform;
		// movement stuff
		XMFLOAT3 accel = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMVECTOR rotQuat;
		XMFLOAT3 size = XMFLOAT3(1.0f, 1.0f, 1.0f);
		XMFLOAT3 vel = XMFLOAT3(1.0f, 1.0f, 1.0f);
		float mass = 1.0f;
		// public methods
	public:
		/*
		* Constructor
		* Params:
		*	std::vector<XMFLOAT3> pointList: the points to make the rigidbody out of
		* Returns: an instance of the class
		*/
		Rigidbody(Vertex vertices[], Transform incomingTransform);
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
		void ApplyFriction(float incomingFrictionCoefficient = 0.1f);
		/*
		* Applies a force to the rigidbody
		* ARGUMENTS: XMFLOAT3 incomingForce -- force being applied to the rigidbody
		* OUTPUT: NONE
		*/
		void ApplyForce(XMFLOAT3 incomingForce);
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
		void Update();
		/*
		* adds the two XMFLOAT3 components and return an XMFLOAT3
		* Arguments:
		*	two XMFLOAT3's to add
		* Returns: added XMFLOAT3
		*/
		XMFLOAT3 AddFloat3(XMFLOAT3 a, XMFLOAT3 b);

		/*
		* subtracts the two XMFLOAT3 components and return an XMFLOAT3
		* Arguments:
		*	two XMFLOAT3's to subtract, in the order the would be subtracted (a - b)
		* Returns: subtracted XMFLOAT3
		*/
		XMFLOAT3 SubFloat3(XMFLOAT3 a, XMFLOAT3 b);
		/*
		* divides an XMFLOAT3 by a scalar
		* Arguments:
		*	the XMFLOAT3 to divide and the float scalar to divide the XMFLOAT3 by
		* Returns: divided XMFLOAT3
		*/
		XMFLOAT3 DivFloat3(XMFLOAT3 float3, float scalar);
		/*
		* multiplies an XMFLOAT3 by a scalar
		* Arguments:
		*	the XMFLOAT3 to multiply and the float scalar to multiply the XMFLOAT3 by
		* Returns: multiplied  XMFLOAT3
		*/
		XMFLOAT3 MultFloat3(XMFLOAT3 float3, float scalar);
		/*
		* finds the magnitude of a XMFLOAT3
		* Arguments:
		*	XMFLOAT3 to find the magnitude of
		* Returns: magnitude of XMFLOAT3
		*/
		float MagFloat3(XMFLOAT3 float3);
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
		XMFLOAT3 GetCollidingColour();
		void SetCollidingColour(XMFLOAT3 value);
		// not colliding colour
		XMFLOAT3 GetNotCollidingCOlour();
		void SetNotCollidingColour(XMFLOAT3 value);
		// local variables
		XMFLOAT3 GetCentreLocal();
		XMFLOAT3 GetMinLocal();
		XMFLOAT3 GetMaxLocal();

		//global variables
		XMFLOAT3 GetCenterGlobal();
		XMFLOAT3 GetMinGlobal();
		XMFLOAT3 GetMaxGlobal();
		// halfWidth 
		XMFLOAT3 GetHalfWidth();

		// matrix model
		XMFLOAT4X4 GetModelMatrix();
		void SetModelMatrix(XMFLOAT4X4 ModelMatrix);
		// collision array stuff
		Rigidbody* GetCollidingArray();
		int GetCollidingCount();
		// position getter and setter
		void SetPosition(XMFLOAT3 incomingPos);
		XMFLOAT3 GetPosition();
		// rotation getter and setter
		void SetRotation(XMVECTOR incomingRot);
		XMVECTOR GetRotation();
		// size getter and setter
		void SetSize(XMFLOAT3 incomingSize);
		XMFLOAT3 GetSize();
		// velocity getter and setter
		void SetVelocity(XMFLOAT3 incomingVel);
		XMFLOAT3 GetVelocity();
		// mass getter and setter
		void SetMass(float incomingMass);
		float GetMass();
#pragma endregion
	};
}