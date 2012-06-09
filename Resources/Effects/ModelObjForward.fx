/**
	File: ModelObj.fx
	Created on: 2012-06-09

	Render a 3D mesh with blending and without lighting.
*/


/** Input Layouts & Global Variables */
struct VS_INPUT
{
	float3 PositionL : POSITION;
	float3 NormalL : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};


cbuffer cbEveryFrame
{
	matrix	gMVP;
};

Texture2D gTexture;
float gAlpha;


/** Render states */
RasterizerState NoCulling
{
	CullMode = None;
	//FillMode = Wireframe;
};

SamplerState LinearSampler 
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

BlendState AdditiveBlending
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
	SrcBlendAlpha = ZERO;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};


/** Shader implementation */
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.PositionH = mul(float4(input.PositionL, 1.0), gMVP);
	output.TexCoord = input.TexCoord;

	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET0
{
	/*
	float2 GBufferUV = (input.PositionH.xy + 1.0f) * 0.5f;
	if (gDepthBuffer.Sample(LinearSampler, GBufferUV).r > input.PositionH.z)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		return gTexture.Sample(LinearSampler, input.TexCoord) * gAlpha;
	}
	*/
	return float4(gTexture.Sample(LinearSampler, input.TexCoord).xyz, gAlpha);
}


/** Technique definitions */
technique10 DrawTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));

		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0xff);
		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	}
}