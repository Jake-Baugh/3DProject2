#ifndef MODELOBJ_HPP
#define MODELOBJ_HPP

#include <string>
#include <map>
#include <Helper\Global.hpp>
#include <Resources\StaticModelData.hpp>
#include <Framework\Effect\Effect.hpp>
#include <Helper\Camera.hpp>

namespace Resources
{
	class ModelObj
	{
	public:
		ModelObj(ID3D10Device* device, const std::string& filename);

		// Bind the model's associated vertex buffer to an input slot
		void Bind(unsigned int slot = 0);

		// Draw the object. Must be bound to an input slot.
		void Draw(const D3DXVECTOR3& drawPosition, const Helper::Camera& camera);
		void Draw(const D3DXMATRIX& modelMatrix, const Helper::Camera& camera);

		// Scale the object
		void SetScale(float newScale);

		// Set the tint colour of the object
		void SetTintColor(D3DXCOLOR newColor);

	private:
		ID3D10Device* mDevice;
		StaticModelData mData;
		Framework::Effect::Effect mEffect;
		float mScale;
		D3DXCOLOR mTintColor;

		ModelObj(const ModelObj&);
		ModelObj& operator=(const ModelObj&);

		bool Load(const std::string& filename);
		bool LoadMaterial(const std::string& filename);
		void UpdatePositionInMatrix(const D3DXVECTOR3& position);
	};
}
#endif