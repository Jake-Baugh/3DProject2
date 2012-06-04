#ifndef GROUND_HPP
#define GROUND_HPP

#include <Framework\VertexBuffer.hpp>
#include <Framework\Effect\Effect.hpp>
#include <Helper\Camera.hpp>

class Ground
{
public:
	Ground(ID3D10Device* device);

	void Draw(const Helper::Camera& camera);

	static const float C_HALFSIZE;
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

#endif