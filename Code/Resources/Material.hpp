#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <map>
#include <Helper\Global.hpp>
#include <Resources\Texture.hpp>

namespace Resources
{
	// Material
	// Defines the properties of a material and keeps a list of its textures.
	struct Material
	{
		struct Definition
		{
			D3DXVECTOR3 Ambient;		// Ka coefficient, default (0.2, 0.2, 0.2)
			D3DXVECTOR3 Diffuse;		// Kd coefficient, default (0.8, 0.8, 0.8)
			D3DXVECTOR3 Specular;		// Ks coefficient, default (1.0, 1.0, 1.0)
			D3DXVECTOR3 Tf;				// Transmission filter, how much of each color to pass through
			int IlluminationModel;		// illum, 0-10
			//float Opacitiy;				// d or Tr, 1.0-> fully opaque, 0.0-> fully transparent
			float RefractionIndex;		// Ni, optical density, (0.001)1.0-10.0, 1.0-> light doesn't bend
			float SpecularExp;			// Ns, ~0-1000, High value-> concentrated highlight
			//float Sharpness;			// Sharpness of reflections, 0-1000, default 60.0
			Texture* MainTexture;		// Texture that can be sent to the shaders

			Definition(ID3D10Device* device);
			~Definition() throw();

		private:
			Definition(const Definition&);
			Definition& operator=(const Definition&);
		};

		ID3D10Device* mDevice;
		std::map<std::string, Definition*> Materials;

		Material(ID3D10Device* device);
		Material(ID3D10Device* device, const std::string& filename);
		~Material() throw();
		
		const Definition* GetMaterial(const std::string& materialName) const;

	private:
		Material(const Material&);
		Material& operator=(const Material&);
	};
}

#endif