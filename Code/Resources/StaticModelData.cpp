#include <sstream>
#include <fstream>
#include <cassert>
#include <Resources\StaticModelData.hpp>
#include <Helper\r2tk\r2-exception.hpp>

namespace Resources
{
	StaticModelData::StaticModelData(ID3D10Device* device, const std::string& objectFilename)
		: mDevice(device)
		, VertexData(device)
		, mDrawableBox(mDevice, Box)
	{
		if (!Load(objectFilename))
			throw r2ExceptionIOM("Failed to load object/material file: " + objectFilename);
	}

	StaticModelData::StaticModelData(ID3D10Device* device, const std::string& objectFilename, const std::string& materialFilename)
		: mDevice(device)
		, VertexData(device)
		, mDrawableBox(mDevice, Box)
	{
		if (!LoadBuffer(objectFilename))
			throw r2ExceptionIOM("Failed to load object file: " + objectFilename);
		if (!LoadMaterial(materialFilename))
			throw r2ExceptionIOM("Failed to load material file: " + materialFilename);
	}

	StaticModelData::~StaticModelData() throw()
	{
		SafeDelete(MaterialData);
	}

	bool StaticModelData::Load(const std::string& objectFilename)
	{
		std::string materialFile;

		if (!LoadObjectFile(objectFilename, materialFile, MaterialName))
			return false;
		return LoadMaterial(materialFile);
	}

	bool StaticModelData::LoadBuffer(const std::string& filename)
	{
		std::string materialFile;
		std::string materialName;

		return LoadObjectFile(filename, materialFile, materialName);
	}

	bool StaticModelData::LoadMaterial(const std::string& filename)
	{
		try 
		{
			MaterialData = new Material(mDevice, filename);
		}
		catch (r2::Exception::IO& e)
		{
			return false;
		}
		
		return true;
	}

	bool StaticModelData::LoadObjectFile(const std::string& filename,
										 std::string& materialFile, 
										 std::string& materialName)
	{
		std::string objectPath = "Resources/Objects/" + filename;

		std::ifstream file;
		std::vector<D3DXVECTOR3> outPositions;
		std::vector<D3DXVECTOR2> outUVCoords;
		std::vector<D3DXVECTOR3> outNormals;
		std::vector<Vertex> vertices;
	
		file.open(objectPath.c_str(), std::ios_base::in);
	
		if(!file.is_open())
			return false;

		while(!file.eof())
		{
			// Read first line of file.
			std::string line;
			std::getline(file, line);

			// Copy line to a stringstream and copy first word into string key
			std::stringstream streamLine;
			std::string key;

			streamLine.str(line);
			streamLine >> key;

			if(key == "mtllib")
			{
				streamLine >> materialFile;
			}
			else if(key == "v")
			{
				D3DXVECTOR3 currPos;
				streamLine >> currPos.x;
				streamLine >> currPos.y;
				streamLine >> currPos.z;
				outPositions.push_back(currPos);

				Box.Corners[0].X = (currPos.x < Box.Corners[0].X) ? currPos.x : Box.Corners[0].X;
				Box.Corners[0].Y = (currPos.y < Box.Corners[0].Y) ? currPos.y : Box.Corners[0].Y;
				Box.Corners[0].Z = (currPos.z < Box.Corners[0].Z) ? currPos.z : Box.Corners[0].Z;

				Box.Corners[1].X = (currPos.x > Box.Corners[1].X) ? currPos.x : Box.Corners[1].X;
				Box.Corners[1].Y = (currPos.y > Box.Corners[1].Y) ? currPos.y : Box.Corners[1].Y;
				Box.Corners[1].Z = (currPos.z > Box.Corners[1].Z) ? currPos.z : Box.Corners[1].Z;
			}
			else if(key == "vt")
			{
				D3DXVECTOR2 currUV;
				streamLine >> currUV.x;
				streamLine >> currUV.y;
				currUV.y = 1 - currUV.y;
				outUVCoords.push_back(currUV);
			}
			else if(key == "vn")
			{
				D3DXVECTOR3 currNormal;
				streamLine >> currNormal.x;
				streamLine >> currNormal.y;
				streamLine >> currNormal.z;
				outNormals.push_back(currNormal);
			}
			else if(key == "f")
			{
				int pos[3]; 
				int uv[3];
				int norm[3];

				for(int i = 0; i < 3; ++i)
				{
					streamLine >> pos[i];
					streamLine.ignore();
					streamLine >> uv[i];
					streamLine.ignore();
					streamLine >> norm[i];

					Vertex currVertex;
					currVertex.Position = outPositions[pos[i] - 1];
					currVertex.UV = outUVCoords[uv[i] - 1];
					currVertex.Normal = outNormals[norm[i] - 1];
					vertices.push_back(currVertex);
				}
			}
			else if(key == "usemtl")
			{
				streamLine >> materialName;
			}
		}

		Framework::VertexBuffer::Description desc;
		desc.ElementCount = vertices.size();
		desc.ElementSize = sizeof(Vertex);
		desc.FirstElementPointer = &vertices[0];
		desc.Topology = Framework::Topology::TriangleList;
		desc.Usage = Framework::Usage::Default;

		mDrawableBox.SetBox(Box);

		return VertexData.SetData(desc, NULL);
	}

	void StaticModelData::DrawAABB(const Camera::Camera& camera, const D3DXMATRIX& world)
	{
		mDrawableBox.Draw(camera, world);
	}
}