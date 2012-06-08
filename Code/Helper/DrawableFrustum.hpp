#ifndef DRAWABLE_FRUSTUM_HPP
#define DRAWABLE_FRUSTUM_HPP

#include <Framework\VertexBuffer.hpp>
#include <Framework\Effect\Effect.hpp>
#include <Helper\Primitives.hpp>
#include <Camera\Camera.hpp>

namespace Helper
{
	class DrawableFrustum
	{
	public:
		DrawableFrustum(ID3D10Device* device, const Helper::Frustum& frustum, const D3DXVECTOR3& origin, const D3DXVECTOR3& direction);

		void Update(const D3DXVECTOR3& origin, const D3DXVECTOR3& direction);
		void SetFrustum(const Frustum& frustum);

		void Draw(const Camera::Camera& camera);
	private:
		void UpdateWorld();
		D3DXMATRIX GetNormalMatrix(const D3DXVECTOR3& position, const D3DXVECTOR3& normal) const;

		ID3D10Device* mDevice;

		Frustum mFrustum;
		std::vector<D3DXVECTOR3> mFrustumNormals;
		std::vector<D3DXVECTOR3> mFrustumPositions;
		D3DXVECTOR3 mOrigin;
		D3DXVECTOR3 mDirection;
		D3DXMATRIX mWorld;

		Framework::VertexBuffer mNormalBuffer;
		Framework::VertexBuffer mFrustumBuffer;
		Framework::Effect::Effect mEffect;
	};
}

#endif