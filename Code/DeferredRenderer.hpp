#ifndef DEFERRED_RENDERER_HPP
#define DEFERRED_RENDERER_HPP

#include <vector>
#include <map>
#include <Framework\D3DContext.hpp>
#include <Framework\Effect\Effect.hpp>
#include <Framework\VertexBuffer.hpp>
#include <Helper\Global.hpp>
#include <Camera\Camera.hpp>

struct DirectionalLight
{
	D3DXVECTOR4 Direction;
	D3DXVECTOR4 Intensity;
};

struct PointLight
{
	D3DXVECTOR4 Position;
	D3DXVECTOR3 Intensity;
	float Radius;
};

class DeferredRenderer 
{
public:
	typedef int LightID;

	DeferredRenderer(Framework::D3DContext* d3dContext, int width, int height);
	~DeferredRenderer() throw();

	void SetDirectionalLight(const DirectionalLight& light);
	size_t AddPointLight(const PointLight& light);
	void SetAmbientLight(const D3DXVECTOR3& light);

	void BeginDeferredState();
	void EndDeferredState();
	void ApplyLightingPhase(const Camera::Camera& camera);

	ID3D10ShaderResourceView* GetFinalComposition();
private:
	struct QuadVertex
	{
		D3DXVECTOR2 Position;
		D3DXVECTOR2 TexCoord;
	};

	ID3D10Texture2D* CreateTexture(DXGI_FORMAT format, UINT bindFlags) const;
	ID3D10RenderTargetView* CreateRenderTargetView(ID3D10Texture2D* buffer) const;
	ID3D10ShaderResourceView* CreateShaderResourceView(ID3D10Texture2D* buffer, DXGI_FORMAT format) const;


	Framework::D3DContext* mD3DContext;
	ID3D10Device* mDevice;
	int mWidth;
	int mHeight;
	//int mColorBufferToRead;
public:
	// The buffers holding the G-buffer data.
	//ID3D10Texture2D* mColorBuffer[2];
	ID3D10Texture2D* mTargetBuffer;
	ID3D10Texture2D* mColorBuffer;
	ID3D10Texture2D* mPositionBuffer;
	ID3D10Texture2D* mNormalBuffer;
	ID3D10Texture2D* mMaterialBuffer;
	ID3D10Texture2D* mDepthStencilBuffer;

	// Render/Depth views for rendering to the buffers
	//ID3D10RenderTargetView* mColorView[2];
	ID3D10RenderTargetView* mTargetView;
	ID3D10RenderTargetView* mColorView;
	ID3D10RenderTargetView* mPositionView;
	ID3D10RenderTargetView* mNormalView;
	ID3D10RenderTargetView* mMaterialView;
	ID3D10DepthStencilView* mDepthStencilView;

	// Shader resource views for reading from the buffers
	//ID3D10ShaderResourceView* mColorSRV[2];
	ID3D10ShaderResourceView* mTargetSRV;
	ID3D10ShaderResourceView* mColorSRV;
	ID3D10ShaderResourceView* mPositionSRV;
	ID3D10ShaderResourceView* mNormalSRV;
	ID3D10ShaderResourceView* mMaterialSRV;
	ID3D10ShaderResourceView* mDepthStencilSRV;

	// Contains the G buffers: color, normal and depth.
	std::vector<ID3D10Texture2D*> mGBuffers;

	// Contains the G buffer render targets: color and normal
	std::vector<ID3D10RenderTargetView*> mRenderTargets;

	// Contains the G buffer shader resource views: color, normal and depth.
	std::vector<ID3D10ShaderResourceView*> mShaderResourceViews;


	// Maps ID to lights
	DirectionalLight mDirectionalLight;
	std::vector<PointLight> mPointLights;
	D3DXVECTOR3 mAmbientLight;

	Framework::Effect::Effect mLightEffect;
	//Framework::Effect::Effect mPointLightEffect;

	Framework::VertexBuffer mFullscreenQuad;
};

#endif