/**
	File: SSAOBlur.fx
	Created on: 2012-06-11

	Inspired by Frank D. Luna
*/

/** Input Layouts & Global Variables */
struct VS_INPUT
{
	float3 PositionH : POSITION;
	float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
};

cbuffer cbEveryFrame
{
	float gTexelWidth;
	float gTexelHeight;
	matrix gView;
	matrix gProjection;
};

cbuffer cbSettings
{
	static const int gBlurRadius = 5;
	float gWeights[11] = 
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
	};
};

Texture2D gNormalBuffer;
Texture2D gDepthBuffer;
Texture2D gInputImage;

/** Render states */
RasterizerState StandardRasterState
{
	CullMode = None;
};

SamplerState NormalSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState DepthSampler 
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Border;
	AddressV = Border;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 1e5f);
};

SamplerState ImageSampler
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;

	AddressU = CLAMP;
	AddressV = CLAMP;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.PositionH = float4(input.PositionH, 1.0f);
	output.TexCoord = input.TexCoord;

	return output;
}

float4 PS(PS_INPUT input, uniform bool gHorizontalBlur) : SV_TARGET0
{
	float2 texOffset;
	if (gHorizontalBlur)
	{
		texOffset = float2(gTexelWidth, 0.0f);
	}
	else
	{
		texOffset = float2(0.0f, gTexelHeight);
	}

	float4 color = gWeights[5] * gInputImage.Sample(ImageSampler, input.TexCoord);
	float totalWeight = gWeights[5];
	
	float3 centerNormal = mul(float4(gNormalBuffer.Sample(NormalSampler, input.TexCoord).xyz, 1.0f), gView).xyz;
	
	
	float centerDepth = gDepthBuffer.Sample(DepthSampler, input.TexCoord).r;
	centerDepth = gProjection._43 / (centerDepth - gProjection._33);
		
	float4 centerNormalDepth = float4(centerNormal, centerDepth);



	for (float i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		// We already added in the center weight.
		if ( i == 0 )
		{
			continue;
		}

		float2 tex = input.TexCoord + i * texOffset;

		float3 neighborNormal = mul(float4(gNormalBuffer.Sample(NormalSampler, tex).xyz, 1.0f), gView).xyz;
		float neighborDepth = gDepthBuffer.Sample(DepthSampler, tex).r;
		neighborDepth = gProjection._43 / (neighborDepth - gProjection._33);
		float4 neighborNormalDepth = float4(neighborNormal, neighborDepth);

		if ( dot(neighborNormal, centerNormal) >= 0.8f &&
			 abs(neighborDepth - centerDepth) <= 0.2f)
		{
			float weight = gWeights[i + gBlurRadius];

			// Add neighbor pixel to blur.
			color += weight * gInputImage.Sample(ImageSampler, tex);

			totalWeight += weight;
		}
	}

	return color / totalWeight;
}

/** Technique definitions */
technique10 DrawTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS(true)));

		SetRasterizerState(StandardRasterState);
	}

	pass P1
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS(false)));

		SetRasterizerState(StandardRasterState);
	}
}