#ifndef GROUND_HPP
#define GROUND_HPP

#include <Framework\VertexBuffer.hpp>
#include <Framework\Effect\Effect.hpp>
#include <Camera\Camera.hpp>

namespace Scene
{
	class Ground
	{
	public:
		Ground(ID3D10Device* device, float halfSize);

		void Draw(const Camera::Camera& camera);
	private:
		struct Vertex
		{
			D3DXVECTOR3 Position;
			D3DXVECTOR2 UV;
		};

		ID3D10Device* mDevice;
		Framework::VertexBuffer mVertexBuffer;
		Framework::Effect::Effect mEffect;
	};
}

#endif