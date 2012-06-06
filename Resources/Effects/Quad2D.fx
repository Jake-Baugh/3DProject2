struct VS_INPUT
{
	float2		position	: POSITION;
	float2		uv			: TEXCOORD;
};

struct PS_INPUT
{
	float4		position	: SV_POSITION;
	float2		uv			: TEXCOORD;
};



RasterizerState NoCulling
{
	CullMode = None;
};

SamplerState linearSampler {
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

Texture2D gTexture;


PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.position = float4(float3(input.position, 0.0f), 1.0);
	output.uv = input.uv;

	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET0
{
	return gTexture.Sample(linearSampler, input.uv);
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