#include <Helper\DrawableBox.hpp>

namespace Helper
{
	DrawableBox::DrawableBox(ID3D10Device* device, const Helper::AABB3f& box)
		: mDevice(device)
		, mBox(box)
		, mBuffer(mDevice)
		, mEffect(mDevice, "Resources/Effects/Debug.fx")
	{
		Framework::Effect::InputLayoutVector inputLayout;
		inputLayout.push_back(Framework::Effect::InputLayoutElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT));
		mEffect.GetTechniqueByIndex(0).GetPassByIndex(0).SetInputLayout(inputLayout);

		SetBox(mBox);
	}

	void DrawableBox::SetBox(const Helper::AABB3f& box)
	{
		mBox = box;

		float width = mBox.GetWidth();
		float height = mBox.GetHeight();
		float depth = mBox.GetDepth();

		D3DXVECTOR3 nearVertices[] = { D3DXVECTOR3(mBox.Corners[0].X, mBox.Corners[0].Y, mBox.Corners[0].Z)
									 , D3DXVECTOR3(mBox.Corners[0].X + width, mBox.Corners[0].Y, mBox.Corners[0].Z)
									 , D3DXVECTOR3(mBox.Corners[0].X, mBox.Corners[0].Y + height, mBox.Corners[0].Z)
									 , D3DXVECTOR3(mBox.Corners[0].X + width, mBox.Corners[0].Y + height, mBox.Corners[0].Z) };

		D3DXVECTOR3 farVertices[] =  { D3DXVECTOR3(mBox.Corners[0].X, mBox.Corners[0].Y, mBox.Corners[1].Z)
									 , D3DXVECTOR3(mBox.Corners[0].X + width, mBox.Corners[0].Y, mBox.Corners[1].Z)
									 , D3DXVECTOR3(mBox.Corners[0].X, mBox.Corners[0].Y + height, mBox.Corners[1].Z)
									 , D3DXVECTOR3(mBox.Corners[0].X + width, mBox.Corners[0].Y + height, mBox.Corners[1].Z) };

		/*
		D3DXVECTOR3 nearVertices[] = { D3DXVECTOR3(mBox.Corners[0].X - halfWidth, mBox.Corners[0].Y - halfHeight, mBox.Corners[0].Z - halfDepth)
									 , D3DXVECTOR3(mBox.Corners[0].X + halfWidth, mBox.Corners[0].Y - halfHeight, mBox.Corners[0].Z - halfDepth)
									 , D3DXVECTOR3(mBox.Corners[0].X - halfWidth, mBox.Corners[0].Y + halfHeight, mBox.Corners[0].Z - halfDepth)
									 , D3DXVECTOR3(mBox.Corners[0].X + halfWidth, mBox.Corners[0].Y + halfHeight, mBox.Corners[0].Z - halfDepth) };

		D3DXVECTOR3 farVertices[] =  { D3DXVECTOR3(-halfWidth, -halfHeight, halfDepth)
									 , D3DXVECTOR3(halfWidth, -halfHeight, halfDepth)
									 , D3DXVECTOR3(-halfWidth, halfHeight, halfDepth)
									 , D3DXVECTOR3(halfWidth, halfHeight, halfDepth) };
		*/

		
		D3DXVECTOR3 vertices[] = { nearVertices[0], nearVertices[1]
								 , nearVertices[1], nearVertices[3]
								 , nearVertices[3], nearVertices[2]
								 , nearVertices[2], nearVertices[0]
								 , nearVertices[0], farVertices[0]
								 , nearVertices[1], farVertices[1]
								 , nearVertices[2], farVertices[2]
								 , nearVertices[3], farVertices[3]
								 , farVertices[0], farVertices[1]
								 , farVertices[1], farVertices[3]
								 , farVertices[3], farVertices[2]
								 , farVertices[2], farVertices[0] };

		Framework::VertexBuffer::Description bufferDescription;
		bufferDescription.ElementCount = sizeof(vertices) / sizeof(vertices[0]);
		bufferDescription.ElementSize = sizeof(D3DXVECTOR3);
		bufferDescription.FirstElementPointer = vertices;
		bufferDescription.Topology = Framework::Topology::LineList;
		bufferDescription.Usage = Framework::Usage::Default;

		mBuffer.SetData(bufferDescription, NULL);
	}

	void DrawableBox::Draw(const Camera::Camera& camera, const D3DXMATRIX& world)
	{
		//D3DXMATRIX translation;
		//D3DXMatrixTranslation(&translation, mBox.Corners[0].X, mBox.Corners[0].Y, mBox.Corners[0].Z);

		mEffect.SetVariable("gWorld", /*translation * */world);
		mEffect.SetVariable("gMVP", /*translation * */world * camera.GetViewProjection());

		mBuffer.Bind();
		for (unsigned int p = 0; p < mEffect.GetTechniqueByIndex(0).GetPassCount(); ++p)
		{
			mEffect.GetTechniqueByIndex(0).GetPassByIndex(p).Apply(mDevice);
			mDevice->Draw(mBuffer.GetElementCount(), 0);
		}
	}
}