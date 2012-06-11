#ifndef SSAO_HPP
#define SSAO_HPP

#include <vector>
#include <Helper\Global.hpp>
#include <Camera\Camera.hpp>
#include <Framework\VertexBuffer.hpp>
#include <Framework\Effect\Effect.hpp>

class SSAO
{
public:
	SSAO(ID3D10Device* device, int backBufferWidth, int backBufferHeight);

	void DrawSSAOBuffer(const Camera::Camera& camera, const Helper::Frustum& frustum, ID3D10ShaderResourceView* positionBuffer, ID3D10ShaderResourceView* normalBuffer, ID3D10ShaderResourceView* depthBuffer);
	void BlurSSAOBuffer(const Camera::Camera& camera, ID3D10ShaderResourceView* normalBuffer, ID3D10ShaderResourceView* depthBuffer, ID3D10ShaderResourceView* SSAOBuffer);
private:
	static const D3DXMATRIX C_TEXTURE_MATRIX;

	struct Vertex
	{
		D3DXVECTOR3 Position;
		D3DXVECTOR3 FrustumCornerIndex;
		D3DXVECTOR2 TexCoord;
	};

	ID3D10Device* mDevice;

	Framework::VertexBuffer mFullscreenQuad;
	Framework::Effect::Effect mEffect;
	Framework::Effect::Effect mBlurEffect;
	ID3D10ShaderResourceView* mRandomTextureSRV;

	std::vector<D3DXVECTOR4> mOffsets;
	std::vector<D3DXVECTOR4> mFrustumFarCorners;

	D3DXVECTOR2 mScreenSize;

	void BuildRandomTexture();
	void BuildOffsetVectors();
	void BuildFrustumFarCorners(const Helper::Frustum& frustum);
};

#endif