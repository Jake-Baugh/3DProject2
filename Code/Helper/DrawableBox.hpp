#ifndef DRAWABLE_BOX_HPP
#define DRAWABLE_BOX_HPP

#include <Helper\Global.hpp>
#include <Helper\Primitives.hpp>
#include <Framework\VertexBuffer.hpp>
#include <Framework\Effect\Effect.hpp>
#include <Camera\Camera.hpp>

namespace Helper
{
	class DrawableBox
	{
	public:
		DrawableBox(ID3D10Device* device, const Helper::AABB3f& box);

		void SetBox(const Helper::AABB3f& box);

		void Draw(const Camera::Camera& camera, const D3DXMATRIX& world);
	private:
		Helper::AABB3f mBox;

		ID3D10Device* mDevice;

		Framework::VertexBuffer mBuffer;
		Framework::Effect::Effect mEffect;
	};
}
#endif