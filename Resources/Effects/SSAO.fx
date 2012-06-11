/**
	File: SSAO.fx
	Created on: 2012-06-09

	Inspired by GameDev.net
*/


/** Input Layouts & Global Variables */
struct VS_INPUT
{
	float3 PositionH : POSITION;
	float3 FrustumCornerIndex : NORMAL;
	float2 TexCoord : TEXCOORD;
};

struct PS_INPUT
{
	float4 PositionH : SV_POSITION;
	float3 ToFarPlane : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
};


cbuffer cbEveryFrame
{
	matrix gView;
	matrix gProjTex;
	float4 gOffsetVectors[14];
	float4 gFrustumFarCorners[4];

	int gScreenWidth;
	int gScreenHeight;

	// SSAO constants
	int gSampleCount = 14;
	float gOcclusionRadius = 0.5f;
	float gOcclusionFadeStart = 0.2f;
	float gOcclusionFadeEnd = 2.0f;
	float gSurfaceEpsilon = 0.05f;
};

Texture2D gPositionBuffer;
Texture2D gNormalBuffer;
Texture2D gDepthBuffer;
Texture2D gRandomBuffer;


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

SamplerState RandomSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};


/** Helper functions */

/** SSAO from GameDev */
float3 GetPosition(float2 uv)
{
	float4 pos = float4(gPositionBuffer.Sample(NormalSampler, uv).xyz, 1.0f);

	return mul(pos, gView).xyz;
}
float3 GetNormal(float2 uv)
{
	float4 nor = float4(gNormalBuffer.Sample(NormalSampler, uv).xyz, 0.0f);

	nor = mul(nor, gView);

	return normalize(nor.xyz * 2.0f - 1.0f);
}
float2 GetRandom(float2 uv)
{
	float2 screenSize = float2(gScreenWidth, gScreenHeight);
	
	float2 rand = gRandomBuffer.Sample(RandomSampler, screenSize * uv / 256.0f).xy;
	return normalize(rand * 2.0f - 1.0f);
}


float DoAmbientOcclusion(float2 tcoord, float2 uv, float3 p, float3 cnorm)
{
	float scale = 1.0f;
	float bias = 0.0f;
	float intensity = 1.0f;

	float3 diff = GetPosition(tcoord + uv) - p;
	const float3 v = normalize(diff);
	const float d = length(diff) * scale;

	return max(0.0f, dot(cnorm, v) - bias * (1.0f / (1.0f + d)) * intensity);
}

float4 SSAOGameDev(PS_INPUT input)
{
	float sampleRad = 1.0f;
	
	const float2 vec[4] = { float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1) };
	float3 positionV = GetPosition(input.TexCoord);
	float3 normalV = GetNormal(input.TexCoord);
	float2 rand = GetRandom(input.TexCoord);

	float ao = 0.0f;
	float rad = sampleRad / positionV.z;

	//** SSAO Calculation */
	int iterations = 4;
	for (int j = 0; j < iterations; ++j)
	{
		float2 coord1 = reflect(vec[j], rand) * rad;
		float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);

		ao += DoAmbientOcclusion(input.TexCoord, coord1 * 0.25f, positionV.xyz, normalV.xyz);
		ao += DoAmbientOcclusion(input.TexCoord, coord2 * 0.50f, positionV.xyz, normalV.xyz);
		ao += DoAmbientOcclusion(input.TexCoord, coord1 * 0.75f, positionV.xyz, normalV.xyz);
		ao += DoAmbientOcclusion(input.TexCoord, coord2 * 1.0f, positionV.xyz, normalV.xyz);
	}

	ao /= (float)iterations * 4.0f;

	return float4(ao, ao, ao, 1.0f);
}



/** Shader implementation */
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.PositionH = float4(input.PositionH, 1.0f);
	output.ToFarPlane = gFrustumFarCorners[input.FrustumCornerIndex.x].xyz;
	output.TexCoord = input.TexCoord;

	return output;
}


float4 PS(PS_INPUT input) : SV_TARGET0
{
	//return SSAOLuna(input);
	return 1.0f - SSAOGameDev(input);
}


/** Technique definitions */
technique10 DrawTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, PS()));

		SetRasterizerState(StandardRasterState);
	}
}