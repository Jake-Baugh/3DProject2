#ifndef DEFERRED_RENDERER_HPP
#define DEFERRED_RENDERER_HPP

#include <vector>
#include <map>
#include <Framework\D3DContext.hpp>
#include <Helper\Global.hpp>

struct DirectionalLight
{
	D3DXVECTOR3 Direction;
	D3DXVECTOR3 Intensity;
};

struct PointLight
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Intensity;
	float Radius;
};

class DeferredRenderer
{
public:
	typedef int LightID;

	DeferredRenderer(Framework::D3DContext* d3dContext, int width, int height);
	~DeferredRenderer() throw();

	void AddDirectionalLight(LightID id, const DirectionalLight& light);
	void AddPointLight(LightID id, const PointLight& light);

	void BeginDeferredState();
	void EndDeferredState();
	void ApplyLightingPhase();
private:
	ID3D10Texture2D* CreateTexture(DXGI_FORMAT format, UINT bindFlags) const;
	ID3D10RenderTargetView* CreateRenderTargetView(ID3D10Texture2D* buffer) const;
	ID3D10ShaderResourceView* CreateShaderResourceView(ID3D10Texture2D* buffer, DXGI_FORMAT format) const;

	void ApplyDirectionalLight(const DirectionalLight& light);
	void ApplyPointLight(const PointLight& light);


	Framework::D3DContext* mD3DContext;
	ID3D10Device* mDevice;
	int mWidth;
	int mHeight;

	// The final buffer we combine the G-buffers + lights into.
	ID3D10Texture2D* mTargetBuffer;

	// The buffers holding the G-buffer data.
	ID3D10Texture2D* mColorBuffer;
	ID3D10Texture2D* mPositionBuffer;
	ID3D10Texture2D* mNormalBuffer;
	ID3D10Texture2D* mMaterialBuffer;
	ID3D10Texture2D* mDepthStencilBuffer;

	// Render/Depth views for rendering to the buffers
	ID3D10RenderTargetView* mTargetView;
	ID3D10RenderTargetView* mColorView;
	ID3D10RenderTargetView* mPositionView;
	ID3D10RenderTargetView* mNormalView;
	ID3D10RenderTargetView* mMaterialView;
	ID3D10DepthStencilView* mDepthStencilView;

	// Shader resource views for reading from the buffers
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
	std::map<LightID, DirectionalLight> mDirectionalLights;
	std::map<LightID, PointLight> mPointLights;
};

#endif