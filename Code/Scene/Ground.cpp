#include <Resources\Texture.hpp>
#include <Scene\Ground.hpp>

namespace Scene
{
	Ground::Ground(ID3D10Device* device, float halfSize)
		: mDevice(device)
		, mVertexBuffer(device)
		, mEffect(device, "Resources/Effects/Ground.fx")
	{
		Framework::Effect::InputLayoutVector inputLayout;
		inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));
		inputLayout.push_back(Framework::Effect::InputLayoutElement("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT));
		mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);

		Ground::Vertex vertices[] = { { D3DXVECTOR3(-halfSize, 0.0f, -halfSize), D3DXVECTOR2(0.0f, 16.0f) }
									, { D3DXVECTOR3(halfSize, 0.0f, -halfSize), D3DXVECTOR2(16.0f, 16.0f) }
									, { D3DXVECTOR3(-halfSize, 0.0f, halfSize), D3DXVECTOR2(0.0f, 0.0f) }
									, { D3DXVECTOR3(halfSize, 0.0f, halfSize), D3DXVECTOR2(16.0f, 0.0f) } };

		Framework::VertexBuffer::Description bufferDescription;
		bufferDescription.ElementCount = 4;
		bufferDescription.ElementSize = sizeof(Ground::Vertex);
		bufferDescription.Topology = Framework::Topology::TriangleStrip;
		bufferDescription.Usage = Framework::Usage::Default;
		bufferDescription.FirstElementPointer = vertices;

		mVertexBuffer.SetData(bufferDescription, NULL);

		mEffect.SetVariable("gModelTexture", Resources::Texture(mDevice, "cobblestone.png").GetShaderResourceView());
		mEffect.SetVariable("gGlowMap", Resources::Texture(mDevice, "glow.png").GetShaderResourceView());
	}

	void Ground::Draw(const Camera::Camera& camera)
	{
		D3DXMATRIX matModel;
		D3DXMatrixIdentity(&matModel);

		mEffect.SetVariable("gMVP", camera.GetViewProjection());
		mEffect.SetVariable("gModel", matModel);

		mVertexBuffer.Bind();
		for (unsigned int p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
		{
			mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
			mDevice->Draw(mVertexBuffer.GetElementCount(), 0);
		}
	}
}