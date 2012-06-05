struct VS_INPUT
{
	float3		positionL	: POSITION;
	float3		normalL		: NORMAL;
	float2		uv			: TEXCOORD;
};

struct PS_INPUT
{
	float4		positionH	: SV_POSITION;
	float3		positionW	: POSITION;
	float3		normalW		: NORMAL;
	float2		uv			: TEXCOORD;
};

struct PS_OUTPUT
{
	float4		color		: SV_TARGET0;
	float4		positionW	: SV_TARGET1;
	float4		material	: SV_TARGET2;
	float4		normalW		: SV_TARGET3;
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
	matrix	gWorld;
	matrix	gMVP;
};

Texture2D gTexture;
float Ka;
float Kd;
float Ks;
float A;


PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.positionH = mul(float4(input.positionL, 1.0), gMVP);
	output.positionW = mul(float4(input.positionL, 1.0), gWorld).xyz;
	output.normalW = mul(float4(input.normalL, 0.0), gWorld).xyz;
	output.uv = input.uv;

	return output;
}

PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT output;

	output.color = gTexture.Sample(linearSampler, input.uv);
	output.positionW = float4(input.positionW, 1.0f);
	output.material = float4(Ka, Kd, Ks, A);
	output.normalW = float4(input.normalW, 1.0f);

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

