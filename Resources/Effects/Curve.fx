struct VS_INPUT
{
	float3		position	: POSITION;
};

struct PS_INPUT
{
	float4		position	: SV_POSITION;
};

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

cbuffer cbEveryFrame
{
	matrix	gMVP;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.position = mul(float4(input.position, 1.0), gMVP);

	return output;
}

float4 PS(PS_INPUT input) : SV_Target0
{
	float4 texColor = float4(1.0f, 0.0f, 0.0f, 1.0f);

	return texColor;
}

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