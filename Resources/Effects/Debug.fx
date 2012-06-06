/**
	File: Debug.fx
	Created on: 2012-06-06

	This shader will render geometry, without normal or texture, in wireframe mode.
	It will, however, render this geometry to deferred G buffers.
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
	float4 Material : SV_TARGET3;
};


cbuffer cbEveryFrame
{
	matrix gMVP;
	matrix gWorld;
};


/** Render states */
RasterizerState StandardRasterState
{
	CullMode = None;
	FillMode = Wireframe;
};


/** Shader implementation */
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.PositionH = mul(float4(input.PositionL, 1.0), gMVP);
	output.PositionW = mul(float4(input.PositionL, 1.0f), gWorld);

	return output;
}

PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output;

	output.Color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	output.PositionW = input.PositionW;
	output.NormalW = float4(0.0f, 1.0f, 0.0f, 1.0f);
	output.Material = float4(0.0f, 0.0f, 0.0f, 0.0f);

	return output;
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