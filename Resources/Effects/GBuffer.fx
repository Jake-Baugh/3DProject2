/**
	File: GBuffer.fx
	Created on: 2012-06-06

	This shader will render a quad on the screen, and sample from a G buffer to fill it up.
	It can be set in depth-mode, if rendering depth, to scale the depth into a visual range.
*/

/** Input Layouts & Global Variables */
struct VS_INPUT
{
	float2 PositionH : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};


Texture2D gBuffer;
bool gDepthMode;


/** Render states */
RasterizerState StandardRasterState
{
	CullMode = None;
};

SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


/** Shader implementation */
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.PositionH = float4(float3(input.PositionH, 0.0), 1.0);
	output.TexCoord = input.TexCoord;

	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET0
{
	float4 color;
	
	if (gDepthMode)
	{
		float depth = gBuffer.Sample(LinearSampler, input.TexCoord);
		depth = pow(depth, 20);
		color = float4(depth, depth, depth, 1.0f);
	}
	else
	{
		color = gBuffer.Sample(LinearSampler, input.TexCoord);
	}

	return color;
}


/** Technique definitions */
technique10 StandardTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));

		SetRasterizerState(StandardRasterState);
	}
}