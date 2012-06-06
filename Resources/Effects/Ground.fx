/**
	File: Ground.fx
	Created on: 2012-06-04

	Render a quad as ground for the world.
*/

/** Input Layouts & Global Variables */
struct VS_INPUT
{
	float3 PositionL : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PS_OUTPUT
{
	float4 Color : SV_TARGET0;
	float4 PositionW : SV_TARGET1;
	float4 NormalW : SV_TARGET2;
	float4 Material : SV_TARGET3;
};


cbuffer cbEveryFrame
{
	matrix gMVP;
	matrix gModel;
};

Texture2D gModelTexture;

float Ka = 1.0;
float Kd = 1.0;
float Ks = 1.0;
float A = 1.0;


/** Render states */
RasterizerState NoCulling
{
	CullMode = None;
	//FillMode = Wireframe;
};

SamplerState LinearSampler {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
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
	output.PositionW = mul(float4(input.PositionL, 1.0), gModel);
	output.TexCoord = input.TexCoord;

	return output;
}

PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output;

	output.Color = gModelTexture.Sample(LinearSampler, input.TexCoord);
	output.PositionW = float4(input.PositionW, 1.0);
	output.NormalW = float4(0.0f, 1.0f, 0.0f, 1.0f);
	output.Material = float4(Ka, Kd, Ks, A);

	return output;
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
	}
}