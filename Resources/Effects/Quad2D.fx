/**
	File: Quad2D.fx
	Created on: 2012-06-04

	Used to render a textured 2D quad onto the screen.
*/


/** Input Layouts & Global Variables */
struct VS_INPUT
{
	float2 PositionH : POSITION;
	float2 TexCoord	: TEXCOORD;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};


Texture2D gTexture;


/** Render states */
RasterizerState NoCulling
{
	CullMode = None;
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

	output.PositionH = float4(float3(input.PositionL, 0.0f), 1.0);
	output.TexCoord = input.TexCoord;

	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET0
{
	return gTexture.Sample(LinearSampler, input.TexCoord);
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