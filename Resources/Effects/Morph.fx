/**
	File: Morph.fx
	Created on: 2012-06-06
*/

/** Input Layouts & Global Variables */
struct VS_INPUT
{
	float3 Position1L : POSITION0;
	float3 Normal1L : NORMAL0;
	float2 TexCoord1 : TEXCOORD0;
	float3 Position2L : POSITION1;
	float3 Normal2L : NORMAL1;
	float2 TexCoord2 : TEXCOORD1;
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

Texture2D gTexture;
Texture2D gGlowMap;
float Ka;
float Kd;
float Ks;
float A;
float t;

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

	float3 pos = lerp(input.Position1L, input.Position2L, t);
	float3 nor = lerp(input.Normal1L, input.Normal2L, t);

	output.PositionH = mul(float4(pos, 1.0), gMVP);
	output.PositionW = mul(float4(pos, 1.0), gWorld);
	output.NormalW = mul(float4(nor, 0.0), gWorld);
	output.TexCoord = lerp(input.TexCoord1, input.TexCoord2, t);

	return output;
}

PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output;

	float3 glowColor = gGlowMap.Sample(LinearSampler, input.TexCoord).xyz;

	output.Color = float4(gTexture.Sample(LinearSampler, input.TexCoord).xyz, glowColor.x);
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