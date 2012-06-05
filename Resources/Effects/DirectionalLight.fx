/**
	File: DirectionalLight.fx
	Created on: 2012-06-05

	This shader will apply a directional light to a deferred scene by rendering a full-screen quad.
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

Texture2D gColorBuffer;
Texture2D gPositionBuffer;
Texture2D gNormalBuffer;
Texture2D gMaterialBuffer;

float3 gLightDirection;
float3 gLightIntensity;
float3 gAmbientLightIntensity;

float3 gEyePositionW;

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


/** Helper functions */
float3 CalculatePhong(float3 N, float3 R, float3 V, float Ka, float Kd, float Ks, float A)
{
	float3 L = -gLightDirection;

	float3 Ia = Ka * gAmbientLightIntensity;
	float3 Id = Kd * saturate(dot(N, L)) * gLightIntensity;
	float3 Is = Ks * exp(saturate(dot(R, V)), A) * gLightIntensity;
	
	return Ia + Id + Is;
}

/** Shader implementation */
PSINPUT VS(VSINPUT input)
{
	PSINPUT output;

	output.PositionH = input.PositionH;
	output.TexCoord = input.TexCoord;

	return output;
}

float4 PS(PSINPUT input) : SV_TARGET0
{
	float3 P = gPositionBuffer.Sample(LinearSampler, input.TexCoord).xyz;
	float3 N = normalize(gNormalBuffer.Sample(LinearSampler, input.TexCoord).xyz);
	float3 R = normalize(reflect(L, N));
	float3 V = normalize(gEyePosition - P);
	float4 Material = gMaterialBuffer.Sample(LinearSampler, input.TexCoord);

	return CalculatePhong(N, R, V, Material.x, Material.y, Material.z, Material.w);
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