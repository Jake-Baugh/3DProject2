/**
	File: Curve.fx
	Created on: 2012-06-05

	Render a line strip in a clear, red color.
*/

/** Input Layouts & Global Variables */
struct VS_INPUT
{
	float3 PositionL : POSITION;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float4 PositionW : POSITION;
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
	matrix gMVP;
	matrix gWorld;
};


/** Render states */
RasterizerState NoCulling
{
	CullMode = None;
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

	output.PositionH = mul(float4(input.PositionL, 1.0f), gMVP);
	output.PositionW = mul(float4(input.PositionL, 1.0f), gWorld);

	return output;
}

PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output;

	output.Color = float4(1.0f, 0.0f, 0.0f, 0.0f);
	output.PositionW = float4(input.PositionW.xyz, 0.0f);
	output.NormalW = float4(0.0f, 1.0f, 0.0f, 0.0f);
	output.Material = float4(0.0f, 0.0f, 0.0f, 0.0f);

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