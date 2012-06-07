#ifndef STATIC_MODEL_DATA_HPP
#define STATIC_MODEL_DATA_HPP

#include <string>
#include <Helper\Global.hpp>
#include <Framework\VertexBuffer.hpp>
#include <Resources\Material.hpp>
#include <Helper\DrawableBox.hpp>
#include <Camera\Camera.hpp>

namespace Resources
{
	struct StaticModelData
	{
		// The struct for a vertex in the object. 
		// The vertex has a position, a texture coordinate and a normal
		struct Vertex
		{
			D3DXVECTOR3 Position;
			D3DXVECTOR3 Normal;
			D3DXVECTOR2	UV;
		};

		Framework::VertexBuffer VertexData;
		Material* MaterialData;
		std::string MaterialName;
		Helper::AABB3f Box;

		StaticModelData(ID3D10Device* device, const std::string& objectFilename);
		StaticModelData(ID3D10Device* device, const std::string& objectFilename, const std::string& materialFilename);
		~StaticModelData() throw();

		bool Load(const std::string& objectFilename);
		bool LoadBuffer(const std::string& filename);
		bool LoadMaterial(const std::string& filename);

		void DrawAABB(const Camera::Camera& camera, const D3DXMATRIX& world);

	private:
		ID3D10Device* mDevice;
		Helper::DrawableBox mDrawableBox;

		// Loads the data from an .obj file.
		// vertices, materialFile and materialName are all
		// out parameters.
		bool LoadObjectFile(const std::string& filename,
							std::string& materialFile, 
							std::string& materialName);

		StaticModelData(const StaticModelData&);
		StaticModelData& operator=(const StaticModelData&);
	};
}

#endif