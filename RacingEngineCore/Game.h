#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"
#include <DirectXMath.h>
#include <vector>

#include "BufferStructs.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void LoadTextures();
	void CreateSampler();
	void LoadMaterials();
	void LoadMeshes();
	void CreateEntities();
	void CreateLights();

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateSkybox(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

	// Sampler
	ID3D11SamplerState* sampler;

	// Sky raster state
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> skyRasterState;
	// Line raster state
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> lineRasterState;

	// Depth Stencil State
	ID3D11DepthStencilState* skyDepthState;

	// Skybox Entity
	Entity* skyboxEntity;

	
	// Skybox texture loaded
	Microsoft::WRL::ComPtr < ID3D11ShaderResourceView> skybox;

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* normalMapVS;
	SimplePixelShader* normalMapPS;
	SimpleVertexShader* skyVS;
	SimplePixelShader* skyPS;

	SimplePixelShader* colorPS; 

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	std::vector<Mesh*> meshes;
	std::vector<Entity*> entities;
	std::vector<Material*> materials;

	Camera* cam;
	DirectionalLight mainLight;

	// Textures
	ID3D11ShaderResourceView* rocks;
	ID3D11ShaderResourceView* rocksNM;
	ID3D11ShaderResourceView* metal;

};

