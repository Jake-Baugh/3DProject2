/**
	File: SSAO.fx
	Created on: 2012-06-09

	Inspired by Frank D. Luna
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

/**
	Make a linear decrement of occlusion by distance.
	O = f(distZ)
*/
float OcclusionFunction(float distZ)
{
	float occlusion = 0.0f;

	if (distZ > gSurfaceEpsilon)
	{
		float fadeLength = gOcclusionFadeEnd - gOcclusionFadeStart;
		occlusion = saturate( (gOcclusionFadeEnd - distZ) / fadeLength);
	}

	return occlusion;
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
	float4 output;

	//float3 positionW = gPositionBuffer.Sample(NormalSampler, input.TexCoord).xyz;
	//float sourceDepth = mul(float4(positionW, 1.0f), gView).z;

	float sourceDepth = gDepthBuffer.Sample(DepthSampler, input.TexCoord).r;									// pz
	float3 sourceNormalV = normalize(mul(float4(gNormalBuffer.Sample(NormalSampler, input.TexCoord).xyz, 0.0f), gView));	// n
	float3 sourcePositionV = (sourceDepth / input.ToFarPlane.z) * input.ToFarPlane;								// p

	float3 randVec = 2.0f * gRandomBuffer.Sample(RandomSampler, 4.0f * input.TexCoord).rgb - 1.0f;				// [0, 1] -> [-1, 1]

	float occlusionSum = 0.0f;
	for (int i = 0; i < gSampleCount; ++i)
	{
		float3 offset = reflect(gOffsetVectors[i].xyz, randVec);
		float flip = sign(dot(offset, sourceNormalV));

		float3 samplePointV = sourcePositionV + flip * gOcclusionRadius * offset;								// q

		float4 samplePointT = mul(float4(samplePointV, 1.0f), gProjTex);										// projQ
		samplePointT /= samplePointT.w;

		float rz = gDepthBuffer.Sample(DepthSampler, samplePointT.xy).r;										// rz

		float3 r = (rz / samplePointV.z) * samplePointV;														// r

		float distZ = sourcePositionV.z - r.z;																	// distZ
		float dp = max(dot(sourceNormalV, normalize(r - sourcePositionV)), 0.0f);								// dp
		float occlusion = dp * OcclusionFunction(distZ);														// occlusion

		occlusionSum += occlusion;
	}

	occlusionSum /= gSampleCount;

	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	return float4(occlusionSum, occlusionSum, occlusionSum, 1.0f);
	
	//float access = 1.0f - occlusionSum;																			// access

	//return saturate(pow(occlusionSum, 4.0f));
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