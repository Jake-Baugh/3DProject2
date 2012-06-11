/**
	File: ModelObj.fx
	Created on: ???

	Modified to use deferred 2012-06-05. Will render a 3D mesh (textured w/ material) into deferred G buffers.
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
	float4 PositionW : POSITION;
	float4 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct PS_OUTPUT
{
	float4 Color : SV_TARGET0;
	float4 PositionW : SV_TARGET1;
	float4 NormalW : SV_TARGET2;
	float4 Material	: SV_TARGET3;
};


cbuffer cbEveryFrame
{
	matrix	gWorld;
	matrix	gMVP;
};

float4 gTintColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
Texture2D gTexture;
Texture2D gGlowMap;
float Ka;
float Kd;
float Ks;
float A;


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
	output.PositionW = mul(float4(input.PositionL, 1.0), gWorld);
	output.NormalW = mul(float4(input.NormalL, 0.0), gWorld);
	output.TexCoord = input.TexCoord;

	return output;
}

PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output;

	float3 glowColor = gGlowMap.Sample(LinearSampler, input.TexCoord).xyz;

	output.Color = float4(gTexture.Sample(LinearSampler, input.TexCoord).xyz, glowColor.x) * gTintColor;
	output.PositionW = float4(input.PositionW.xyz, glowColor.y);
	output.NormalW = float4(input.NormalW.xyz, glowColor.z);
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

