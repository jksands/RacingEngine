#include "Mesh.h"
#include <fstream>
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "includes/tiny_obj_loader.h"
#include <vector>
#include <iostream>
#include <assert.h>

using namespace DirectX;

/// <summary>
/// Creates Vertex and Index buffers from the provided parameters
/// </summary>
/// <param name="vertices">Vertices of the mesh</param>
/// <param name="vertexAmt"># of vertices present</param>
/// <param name="_indices">indices in which to draw the vertices</param>
/// <param name="indexAmt">Amount of indices to draw</param>
/// <param name="buffer">A pointer to the device to create the buffer</param>
Mesh::Mesh(Vertex vertices[], 
    int vertexAmt, 
    UINT _indices[], 
    int indexAmt, 
    ID3D11Device* device)
{
	// CalculateTangents(vertices, vertexAmt, _indices, indexAmt);
    CreateBuffer(vertices, vertexAmt, _indices, indexAmt, device);
	// Keep verts around for bounding box generation
	for (int i = 0; i < vertexAmt; i++)
	{
		verts.push_back(vertices[i]);
	}
}

Mesh::Mesh(const char* objFile, ID3D11Device* device)
{
	// TinyOBJLoad(objFile, device);
	// File input object
	std::ifstream obj(objFile);

	// Check for successful open
	if (!obj.is_open())
		return;

	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	std::vector<Vertex> verts;           // Verts we're assembling
	std::vector<UINT> indices;           // Indices of these verts
	unsigned int vertCounter = 0;        // Count of vertices/indices
	char chars[100];                     // String for line reading

	// Still have data left?
	while (obj.good())
	{
		// Get the line (100 characters should be more than enough)
		obj.getline(chars, 100);

		// Check the type of line
		if (chars[0] == 'v' && chars[1] == 'n')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(
				chars,
				"vn %f %f %f",
				&norm.x, &norm.y, &norm.z);

			// Add to the list of normals
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't')
		{
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(
				chars,
				"vt %f %f",
				&uv.x, &uv.y);

			// Add to the list of uv's
			uvs.push_back(uv);
		}
		else if (chars[0] == 'v')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(
				chars,
				"v %f %f %f",
				&pos.x, &pos.y, &pos.z);

			// Add to the positions
			positions.push_back(pos);
		}
		else if (chars[0] == 'f')
		{
			// Read the face indices into an array
			// NOTE: This assumes the given obj file contains
			//  vertex positions, uv coordinates AND normals.
			//  If the model is missing any of these, this 
			//  code will not handle the file correctly!
			unsigned int i[12];
			int facesRead = sscanf_s(
				chars,
				"f %u/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8],
				&i[9], &i[10], &i[11]);
			//for (int j = 0; j < 12; j++)
			//{
			//	// some obj files might have negative f's
			//	unsigned int temp = 1 << 31;
			//	temp = temp | i[j];
			//	i[j] = i[j] & temp;
			//}
			// - Create the verts by looking up
			//    corresponding data from vectors
			// - OBJ File indices are 1-based, so
			//    they need to be adusted
			Vertex v1;
			v1.Position = positions[i[0] - 1];
			v1.UV = uvs[i[1] - 1];
			v1.Normal = normals[i[2] - 1];

			Vertex v2;
			v2.Position = positions[i[3] - 1];
			v2.UV = uvs[i[4] - 1];
			v2.Normal = normals[i[5] - 1];

			Vertex v3;
			v3.Position = positions[i[6] - 1];
			v3.UV = uvs[i[7] - 1];
			v3.Normal = normals[i[8] - 1];

			// The model is most likely in a right-handed space,
			// especially if it came from Maya.  We want to convert
			// to a left-handed space for DirectX.  This means we 
			// need to:
			//  - Invert the Z position
			//  - Invert the normal's Z
			//  - Flip the winding order
			// We also need to flip the UV coordinate since DirectX
			// defines (0,0) as the top left of the texture, and many
			// 3D modeling packages use the bottom left as (0,0)

			// Flip the UV's since they're probably "upside down"
			v1.UV.y = 1.0f - v1.UV.y;
			v2.UV.y = 1.0f - v2.UV.y;
			v3.UV.y = 1.0f - v3.UV.y;

			// Flip Z (LH vs. RH)
			v1.Position.z *= -1.0f;
			v2.Position.z *= -1.0f;
			v3.Position.z *= -1.0f;

			// Flip normal Z
			v1.Normal.z *= -1.0f;
			v2.Normal.z *= -1.0f;
			v3.Normal.z *= -1.0f;

			// Add the verts to the vector (flipping the winding order)
			verts.push_back(v1);
			verts.push_back(v3);
			verts.push_back(v2);

			// Add three more indices
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;
			indices.push_back(vertCounter); vertCounter += 1;

			// Was there a 4th face?
			if (facesRead == 12)
			{
				// Make the last vertex
				Vertex v4;
				v4.Position = positions[i[9] - 1];
				v4.UV = uvs[i[10] - 1];
				v4.Normal = normals[i[11] - 1];

				// Flip the UV, Z pos and normal
				v4.UV.y = 1.0f - v4.UV.y;
				v4.Position.z *= -1.0f;
				v4.Normal.z *= -1.0f;

				// Add a whole triangle (flipping the winding order)
				verts.push_back(v1);
				verts.push_back(v4);
				verts.push_back(v3);

				// Add three more indices
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
				indices.push_back(vertCounter); vertCounter += 1;
			}
		}
	}

	// Close the file and create the actual buffers
	obj.close();


	// - At this point, "verts" is a vector of Vertex structs, and can be used
	//    directly to create a vertex buffer:  &verts[0] is the address of the first vert
	//
	// - The vector "indices" is similar. It's a vector of unsigned ints and
	//    can be used directly for the index buffer: &indices[0] is the address of the first int
	//
	// - "vertCounter" is BOTH the number of vertices and the number of indices
	// - Yes, the indices are a bit redundant here (one per vertex).  Could you skip using
	//    an index buffer in this case?  Sure!  Though, if your mesh class assumes you have
	//    one, you'll need to write some extra code to handle cases when you don't.
	// CalculateTangents(&verts[0], verts.size(), &indices[0], indices.size());
	CalculateTangents(&verts[0], verts.size(), &indices[0], indices.size());
	CreateBuffer(&verts[0], verts.size(), &indices[0], (int)indices.size(), device);
	// Keep verts around for bounding box generation
	this->verts = verts;
}

//Mesh::Mesh(const char* file, ID3D11Device* buffer, bool useFBX)
//{
//	// only use this one if useFBX is true
//	// throw an exception because programmer is a dumb
//	if (!useFBX)
//	{
//		throw;
//	}
//
//	if (g_pFbxSdkManager == nullptr)
//	{
//		g_pFbxSdkManager = FbxManager::Create();
//
//		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT);
//		g_pFbxSdkManager->SetIOSettings(pIOsettings);
//	}
//
//	FbxImporter* pImporter = FbxImporter::Create(g_pFbxSdkManager, "");
//	FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager, "");
//
//	bool bSuccess = pImporter->Initialize(file, -1, g_pFbxSdkManager->GetIOSettings());
//	if (!bSuccess) throw;
//
//	bSuccess = pImporter->Import(pFbxScene);
//	if (!bSuccess) throw;
//
//	pImporter->Destroy();
//
//	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
//
//	std::vector<MinimumVertex> verts;           // Verts we're assembling
//	std::vector<UINT> indices;           // Indices of these verts
//	unsigned int vertCounter = 0;        // Count of vertices/indices
//	int triCounter = 0;
//	int quadCounter = 0;
//	int nGonCounter = 0;
//
//	if (pFbxRootNode)
//	{
//		int tempor = pFbxRootNode->GetChildCount();
//		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
//		{
//			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);
//
//			if (pFbxChildNode->GetNodeAttribute() == NULL)
//				continue;
//
//			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();
//
//
//			if (AttributeType != FbxNodeAttribute::eMesh)
//				continue;
//
//			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();
//
//			FbxVector4* pVertices = pMesh->GetControlPoints();
//			int num = pMesh->GetPolygonCount();
//			pMesh->GetPolygonVertices();
//			for (int j = 0; j < pMesh->GetPolygonCount(); j++)
//			{
//				FbxVector4 norm;
//				int iNumVertices = pMesh->GetPolygonSize(j);
//				// Gets the normal at a given index
//				if (!pMesh->GetPolygonVertexNormal(j, 0, norm))
//					throw;
//				if (iNumVertices <= 4)
//				{
//					if (iNumVertices == 3)
//					{
//						// continue;
//						triCounter++;
//					}
//					// 
//					MinimumVertex v1;
//					MinimumVertex v2;
//					MinimumVertex v3;
//					int i = pMesh->GetPolygonVertex(j, 0);
//					v1.Position = XMFLOAT3(
//						(float)pVertices[i].mData[0],
//						(float)pVertices[i].mData[1],
//						(float)pVertices[i].mData[2]);
//					i = pMesh->GetPolygonVertex(j, 1);
//					v2.Position = XMFLOAT3(
//						(float)pVertices[i].mData[0],
//						(float)pVertices[i].mData[1],
//						(float)pVertices[i].mData[2]);
//					i = pMesh->GetPolygonVertex(j, 2);
//					v3.Position = XMFLOAT3(
//						(float)pVertices[i].mData[0],
//						(float)pVertices[i].mData[1],
//						(float)pVertices[i].mData[2]);
//					// Flip the Z (LH vs. RH
//					v1.Position.z *= -1.0f;
//					v2.Position.z *= -1.0f;
//					v3.Position.z *= -1.0f;
//
//					verts.push_back(v1);
//					verts.push_back(v3);
//					verts.push_back(v2);
//					// Add three more indices
//					indices.push_back(vertCounter); vertCounter += 1;
//					indices.push_back(vertCounter); vertCounter += 1;
//					indices.push_back(vertCounter); vertCounter += 1;
//
//					if (iNumVertices == 4) // This is a square
//					{
//						quadCounter++;
//						MinimumVertex v4;
//						i = pMesh->GetPolygonVertex(j, 3);
//						v4.Position = XMFLOAT3(
//							(float)pVertices[i].mData[0],
//							(float)pVertices[i].mData[1],
//							(float)pVertices[i].mData[2]);
//						v4.Position.z *= -1.0f;
//						// Add a whole triangle (flipping the winding order)
//						verts.push_back(v1);
//						verts.push_back(v4);
//						verts.push_back(v3);
//
//						// Add three more indices
//						indices.push_back(vertCounter); vertCounter += 1;
//						indices.push_back(vertCounter); vertCounter += 1;
//						indices.push_back(vertCounter); vertCounter += 1;
//					}
//					//for (int k = 0; k < iNumVertices; k++) {
//					//	int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
//
//					//	MinimumVertex vertex;
//					//	vertex.Position = XMFLOAT3(
//					//		(float)pVertices[iControlPointIndex].mData[0],
//					//		(float)pVertices[iControlPointIndex].mData[1],
//					//		(float)pVertices[iControlPointIndex].mData[2]);
//					//	vertex.Position.z *= -1;
//					//	verts.push_back(vertex);
//					//	// Add index
//					//	indices.push_back(vertCounter); vertCounter += 1;
//					//}
//				}
//				else 
//				{
//					// unhandled number of verts?
//					// Just do your best, champ
//					nGonCounter++;
//					continue;
//				}
//
//			}
//
//		}
//
//	}
//	
//	// Generate MINVertex buffer
//
//	// CalculateTangents(&verts[0], verts.size(), &indices[0], indices.size());
//	// CalculateTangents(&verts[0], verts.size(), &indices[0], indices.size());
//	CreateMinVertexBuffer(&verts[0], verts.size(), &indices[0], (int)indices.size(), buffer);
//	// Keep verts around for bounding box generation
//	// this->verts = verts;
//}

void Mesh::TinyOBJLoad(const char* file, ID3D11Device* buffer)
{
	std::string inputfile = file;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0) {
					tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
					tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
					tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0) {
					tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
				}
				// Optional: vertex colors
				// tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
				// tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
				// tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
			}
			index_offset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];
		}
	}
	// Once file is loaded into attrib, create lists out of provided data

	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	for (int i = 0; i < attrib.vertices.size() / 3; i++)
	{
		positions.push_back(XMFLOAT3(attrib.vertices[(i * 3)], attrib.vertices[(i * 3) + 1], attrib.vertices[(i * 3) + 2]));
	}
	int t =  attrib.vertices.size();
	t = attrib.normals.size();
	t = attrib.texcoords.size();
	t = 0;
}

// Creates buffers with information passed from the constructor
void Mesh::CreateBuffer(Vertex vertices[], int vertexAmt, UINT _indices[], int indexAmt, ID3D11Device* device)
{
	// Save the amount of indices for future use
	indices = indexAmt;

	// Create the VERTEX BUFFER description -----------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * vertexAmt;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// Tells DirectX this is a vertex buffer
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;


	// Create the proper struct to hold the initial vertex data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;


	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());

	// Create Index BUffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * indexAmt;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = _indices;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
}

void Mesh::CreateMinVertexBuffer(MinimumVertex vertices[], int vertexAmt, UINT _indices[], int indexAmt, ID3D11Device* device)
{
	// Save the amount of indices for future use
	indices = indexAmt;

	// Create the VERTEX BUFFER description -----------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(MinimumVertex) * vertexAmt;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// Tells DirectX this is a vertex buffer
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;


	// Create the proper struct to hold the initial vertex data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = vertices;


	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());

	// Create Index BUffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * indexAmt;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = _indices;

	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
}

// Calculates the tangents of the vertices in a mesh
// - Code originally adapted from: http://www.terathon.com/code/tangent.html
//   - Updated version now found here: http://foundationsofgameenginedev.com/FGED2-sample.pdf
//   - See listing 7.4 in section 7.5 (page 9 of the PDF)
//
// - Note: For this code to work, your Vertex format must
//         contain an XMFLOAT3 called Tangent
void Mesh::CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices)
{
	// Reset tangents
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].Tangent = XMFLOAT3(0, 0, 0);
	}

	// Calculate tangents one whole triangle at a time
	for (int i = 0; i < numIndices;)
	{
		// Grab indices and vertices of first triangle
		unsigned int i1 = indices[i++];
		unsigned int i2 = indices[i++];
		unsigned int i3 = indices[i++];
		Vertex* v1 = &verts[i1];
		Vertex* v2 = &verts[i2];
		Vertex* v3 = &verts[i3];

		// Calculate vectors relative to triangle positions
		float x1 = v2->Position.x - v1->Position.x;
		float y1 = v2->Position.y - v1->Position.y;
		float z1 = v2->Position.z - v1->Position.z;

		float x2 = v3->Position.x - v1->Position.x;
		float y2 = v3->Position.y - v1->Position.y;
		float z2 = v3->Position.z - v1->Position.z;

		// Do the same for vectors relative to triangle uv's
		float s1 = v2->UV.x - v1->UV.x;
		float t1 = v2->UV.y - v1->UV.y;

		float s2 = v3->UV.x - v1->UV.x;
		float t2 = v3->UV.y - v1->UV.y;

		// Create vectors for tangent calculation
		float r = 1.0f / (s1 * t2 - s2 * t1);

		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		// Adjust tangents of each vert of the triangle
		v1->Tangent.x += tx;
		v1->Tangent.y += ty;
		v1->Tangent.z += tz;

		v2->Tangent.x += tx;
		v2->Tangent.y += ty;
		v2->Tangent.z += tz;

		v3->Tangent.x += tx;
		v3->Tangent.y += ty;
		v3->Tangent.z += tz;
	}

	// Ensure all of the tangents are orthogonal to the normals
	for (int i = 0; i < numVerts; i++)
	{
		// Grab the two vectors
		XMVECTOR normal = XMLoadFloat3(&verts[i].Normal);
		XMVECTOR tangent = XMLoadFloat3(&verts[i].Tangent);

		// Use Gram-Schmidt orthogonalize
		tangent = XMVector3Normalize(
			tangent - normal * XMVector3Dot(normal, tangent));

		// Store the tangent
		XMStoreFloat3(&verts[i].Tangent, tangent);
	}
}

Mesh::~Mesh()
{
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
    return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
    return indexBuffer;
}

int Mesh::GetIndexCount()
{
    return indices;
}

std::vector<Vertex> Mesh::GetVertices()
{
	return verts;
}
