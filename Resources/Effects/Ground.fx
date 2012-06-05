struct VS_INPUT
{
	float3		positionW	: POSITION;
	float2		uv			: TEXCOORD;
};

struct PS_INPUT
{
	float4		positionH	: SV_POSITION;
	float4		positionW	: POSITION;
	float2		uv			: TEXCOORD;
};

struct PS_OUTPUT
{
	float4		color		: SV_TARGET0;
	float4		position	: SV_TARGET1;
	float4		material	: SV_TARGET2;
	float4		normal		: SV_TARGET3;
};



RasterizerState NoCulling
{
	CullMode = None;
	//FillMode = Wireframe;
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



cbuffer cbEveryFrame
{
	matrix	gMVP;
};

Texture2D gModelTexture;



PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.positionH = mul(float4(input.position, 1.0), gMVP);
	output.positionW = input.positionW;
	output.uv = input.uv;

	return output;
}

PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output;

	output.color = gModelTexture.Sample(linearSampler, input.uv);
	output.position = input.positionW;
	//output.material = float4(1.0,
	output.normal = float4(0.0f, 1.0f, 0.0f, 0.0f);

	return output;
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