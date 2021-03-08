#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"
/// <summary>
/// Class to hold Mesh data and vertex and index buffers for each.
/// </summary>
class Mesh
{
public:
	Mesh(Vertex vertices[], int vertexAmt, UINT _indices[], int indexAmt, ID3D11Device* device);
	Mesh(const char* file, ID3D11Device* buffer);
	void CreateBuffer(Vertex vertices[], int vertexAmt, UINT _indices[], int indexAmt, ID3D11Device* device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
	~Mesh();


	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int indices = 0;
	// Might need to keep the vertices used around as well to construct a bounding box out of them
};

