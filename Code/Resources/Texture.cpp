#include <Resources\Texture.hpp>
#include <Helper\r2tk\r2-exception.hpp>

namespace Resources
{
	Texture::Texture(ID3D10Device* device, const std::string& filename)
		: mTexture(NULL)
	{
		std::string path = "Resources/Textures/" + filename;

		if (FAILED(D3DX10CreateShaderResourceViewFromFile(device, path.c_str(), NULL, NULL, &mTexture, NULL)))
			throw r2ExceptionIOM("Failed to load texture: " + path);

		if (FAILED(D3DX10GetImageInfoFromFile(path.c_str(), NULL, &mImageInfo, NULL)))
			throw r2ExceptionIOM("Failed to load texture information: " + path);
	}

	Texture::~Texture() throw()
	{
		SafeRelease(mTexture);
	}

	ID3D10ShaderResourceView* Texture::GetShaderResourceView()
	{
		return mTexture;
	}

	unsigned int Texture::GetWidth() const
	{
		return mImageInfo.Width;
	}

	unsigned int Texture::GetHeight() const
	{
		return mImageInfo.Height;
	}
}