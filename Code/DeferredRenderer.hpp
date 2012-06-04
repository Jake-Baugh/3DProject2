#ifndef DEFERRED_RENDERER_HPP
#define DEFERRED_RENDERER_HPP

#include <vector>
#include <Framework\D3DContext.hpp>
#include <Helper\Global.hpp>

class DeferredRenderer
{
public:
	DeferredRenderer(Framework::D3DContext* d3dContext, int width, int height);
	~DeferredRenderer() throw();

	void BeginDeferredState();
	void EndDeferredState();
private:
	ID3D10Texture2D* CreateTexture(DXGI_FORMAT format, UINT bindFlags) const;
	ID3D10RenderTargetView* CreateRenderTargetView(ID3D10Texture2D* buffer) const;
	ID3D10ShaderResourceView* CreateShaderResourceView(ID3D10Texture2D* buffer, DXGI_FORMAT format) const;

	Framework::D3DContext* mD3DContext;
	ID3D10Device* mDevice;

	ID3D10Texture2D* mColorBuffer;
	ID3D10Texture2D* mNormalBuffer;
	ID3D10Texture2D* mDepthStencilBuffer;

	ID3D10RenderTargetView* mColorView;
	ID3D10RenderTargetView* mNormalView;
	ID3D10DepthStencilView* mDepthStencilView;
public:
	ID3D10ShaderResourceView* mColorSRV;
	ID3D10ShaderResourceView* mNormalSRV;
	ID3D10ShaderResourceView* mDepthStencilSRV;

	std::vector<ID3D10RenderTargetView*> mRenderTargets;

	int mWidth;
	int mHeight;
};

#endif