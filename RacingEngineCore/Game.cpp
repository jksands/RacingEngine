#include "Game.h"
#include "Vertex.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexShader = 0;
	pixelShader = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class

	// Delete Meshes
	for (auto m : meshes)
	{
		delete m;
	}
	// Delete entities
	for (auto e : entities)
	{
		delete  e;
	}
	// Delete materials
	for (auto mat : materials)
	{
		delete mat;
	}
	// Delete camera
	delete cam;

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;
	delete normalMapVS;
	delete normalMapPS;
	delete skyVS;
	delete skyPS;
	delete skyboxEntity;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Create Raster states
	D3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_FRONT;
	rastDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rastDesc, skyRasterState.GetAddressOf());

	// Depth state for accepting pixels with depth EQUAL to existing depth
	D3D11_DEPTH_STENCIL_DESC ds = {};
	ds.DepthEnable = true;
	ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&ds, &skyDepthState);

	// create Skybox
	skybox = CreateSkybox(
		L"../../Assets/Skybox/Right.jpg",
		L"../../Assets/Skybox/Left.jpg",
		L"../../Assets/Skybox/Top.jpg",
		L"../../Assets/Skybox/Bot.jpg",
		L"../../Assets/Skybox/Front.jpg",
		L"../../Assets/Skybox/Back.jpg"
	);
	// Load Shaders
	LoadShaders();
	// Load Textures
	LoadTextures();
	// Create Sampler
	CreateSampler();
	// Load Materials
	LoadMaterials();
	// Load Meshes
	LoadMeshes();
	// Create Entities
	CreateEntities();
	// Create lights
	CreateLights();


	// Create Camera
	cam = new Camera(XMFLOAT3(0, 0, -10), (float)width / height, 2, 10);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");

	normalMapVS = new SimpleVertexShader(device, context);
	normalMapVS->LoadShaderFile(L"NormalMapVS.cso");

	normalMapPS = new SimplePixelShader(device, context);
	normalMapPS->LoadShaderFile(L"NormalMapPS.cso");

	skyVS = new SimpleVertexShader(device, context);
	skyVS->LoadShaderFile(L"SkyVS.cso");

	skyPS = new SimplePixelShader(device, context);
	skyPS->LoadShaderFile(L"SkyPS.cso");
}

void Game::LoadTextures()
{
	CreateWICTextureFromFile(
		device,
		context,
		L"../../Assets/Textures/rock.png",
		nullptr,
		&rocks
	);
	CreateWICTextureFromFile(
		device,
		context,
		L"../../Assets/Textures/rock_normals.png",
		nullptr,
		&rocksNM
	);
	CreateWICTextureFromFile(
		device,
		context,
		L"../../Assets/Textures/metal.png",
		nullptr,
		&metal
	);
}

void Game::CreateSampler()
{
	D3D11_SAMPLER_DESC sDesc = {};
	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sDesc.MaxAnisotropy = 8;
	sDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (device->CreateSamplerState(&sDesc, &sampler) != S_OK) throw; // Simply breaks the program here to show a problem occurred
}

// Creates a basic Material - will load in textures in the future
void Game::LoadMaterials()
{
	materials.push_back(new Material(skyPS, skyVS, XMFLOAT4(1, 1, 1, 0), 0.1f, skybox.Get(), sampler));
	materials.push_back(new Material(normalMapPS, normalMapVS, XMFLOAT4(1, 1, 1, 0), 1.0f, rocks, sampler, rocksNM));
	materials.push_back(new Material(pixelShader, vertexShader, XMFLOAT4(0, 0, 1, 0), 1.0f, metal, sampler));
	materials.push_back(new Material(pixelShader, vertexShader, XMFLOAT4(1, 1, 1, 0), 0.1f));
}

// Loads a mesh from a file - Can create a mesh yourself should you desire
void Game::LoadMeshes()
{
	meshes.push_back(new Mesh("../../Assets/Models/sphere.obj", device));
	meshes.push_back(new Mesh("../../Assets/Models/helix.obj", device));
	meshes.push_back(new Mesh("../../Assets/Models/cube.obj", device));
	meshes.push_back(new Mesh("../../Assets/Models/Car.obj", device));
}

// Creates entities.  Can choose any mesh and any material, and position anywhere
void Game::CreateEntities()
{
	entities.push_back(new Entity(meshes[0], materials[1], Transform(XMFLOAT3(0, 1, 0))));
	entities.push_back(new Entity(meshes[1], materials[2], Transform(XMFLOAT3(1, 0, 0))));
	entities.push_back(new Entity(meshes[2], materials[3], Transform(XMFLOAT3(-1, 0, 0))));
	entities.push_back(new Entity(meshes[3], materials[3], Transform(XMFLOAT3(0, -2, 0), XMFLOAT3(.05f,.05f,.05f))));

	skyboxEntity = new Entity(meshes[2], materials[0]);
}

// Creates lights.
void Game::CreateLights()
{
	mainLight = {};
	mainLight.ambientColor = XMFLOAT3(.1f, .1f, .1f);
	mainLight.diffuseColor = XMFLOAT3(1, 1, 1);	// White light
	mainLight.direction = XMFLOAT3(1, 0, 1);	// Facing same way as camera
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Game::CreateSkybox(
	const wchar_t* right, 
	const wchar_t* left, 
	const wchar_t* up, 
	const wchar_t* down, 
	const wchar_t* front, 
	const wchar_t* back)
{
	// load 6 texture into an array
	// - refer the TEXTURES, not the SRVs!
	// NOT generating mipmaps on purpose as they are not needed
	ID3D11Texture2D* textures[6] = {};
	CreateWICTextureFromFile(device, right, (ID3D11Resource**)&textures[0], 0);
	CreateWICTextureFromFile(device, left, (ID3D11Resource**)&textures[1], 0);
	CreateWICTextureFromFile(device, up, (ID3D11Resource**)&textures[2], 0);
	CreateWICTextureFromFile(device, down, (ID3D11Resource**)&textures[3], 0);
	CreateWICTextureFromFile(device, front, (ID3D11Resource**)&textures[4], 0);
	CreateWICTextureFromFile(device, back, (ID3D11Resource**)&textures[5], 0);

	// We assume all textures are same format and resolution
	// so just get the desc of the FIRST
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);

	// Describe the resource for the cube map, which is just a "texture 2d array"
	// this is a special GPU format, NOT just a C++ array of textures!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6; // cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // Used as a texture in a shader
	cubeDesc.CPUAccessFlags = 0; // No read back
	cubeDesc.Format = faceDesc.Format; // Match the texture's color format
	cubeDesc.Width = faceDesc.Width; // Mathc the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1; // Only need 1;
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // This is a CUBE, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;

	// Create the actual texture resource
	ID3D11Texture2D* cubeMapTexture = 0;
	device->CreateTexture2D(&cubeDesc, 0, &cubeMapTexture);

	// Loop through and copy the individual faces to the texture
	for (int i = 0; i < 6; i++)
	{
		// calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0, // Which mip (zero for this)
			i, // Which array element?
			1); // How many mip levels are in texture?
		// Copy from one resource to another
		context->CopySubresourceRegion(
			cubeMapTexture, // Destination resource
			subresource, // Dest subresource index (one of the array elements)
			0, 0, 0, // XYZ location of copy
			textures[i], //Source resource
			0,  // Source subresource index (assuming there's only one)
			0); // Source subresource "box" of data to copy (zero = whole thing)
	}
	// At this point, all of the faces have been copied to the cube map texture,
	// so we can describe an SRV for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format; // Same as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1; // Only 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0; // Index of the first mip we want to see

	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	device->CreateShaderResourceView(cubeMapTexture, &srvDesc, cubeSRV.GetAddressOf());

	// Now that we're done, clean up the stuff we don't need anymore
	cubeMapTexture->Release(); // Done with this reference (SRV has the other)
	for (int i = 0; i < 6; i++)
		textures[i]->Release();

	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	if (cam)
	{
		// Update camera proj
		cam->UpdateProjectionMatrix((float)width / height);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	cam->Update(deltaTime, this->hWnd);
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	pixelShader->SetShader();

	// Sets the light data in the pixel shader
	pixelShader->SetData(
		"directionalLight",
		&mainLight,
		sizeof(DirectionalLight)
	);

	normalMapPS->SetData(
		"directionalLight",
		&mainLight,
		sizeof(DirectionalLight)
	);

	// DO THIS BETTER! This is in game draw, but the vs updates are all in entity draw.  MAKES NO SENSE
	pixelShader->CopyAllBufferData();
	normalMapPS->CopyAllBufferData();
	
	// Draw all entities
	for (int i = 0; i < entities.size(); i++)
	{
		entities[i]->Draw(context, cam);
	}
	context->RSSetState(skyRasterState.Get());
	context->OMSetDepthStencilState(skyDepthState, 0);
	// DRAW SKYBOX
	skyboxEntity->Draw(context, cam, 'S');
	// Reset raster state
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);




	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion