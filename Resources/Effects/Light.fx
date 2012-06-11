/**
	File: Light.fx
	Created on: 2012-06-05

	This shader will apply a directional light and an array of point lights
    to a deferred scene by rendering a full-screen quad.
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

struct DirectionalLight
{
	float4 DirectionW;
	float4 Intensity;
};

struct PointLight
{
	float4 PositionW;
	float3 Intensity;
	float Radius;
};

matrix gView;
matrix gProjection;

Texture2D gColorBuffer;
Texture2D gPositionBuffer;
Texture2D gNormalBuffer;
Texture2D gMaterialBuffer;
Texture2D gSSAOBuffer;

float3 gAmbientLightIntensity;
DirectionalLight gDirectionalLight;
PointLight gPointLights[100];
int gPointLightCount;

float3 gEyePositionW;

bool gSSAOToggle = true;



/** Render states */
RasterizerState StandardRasterState
{
	CullMode = None;
};

BlendState NoBlending
{
	BlendEnable[0] = FALSE;
};

SamplerState LinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


/** Helper functions */
/*
float3 CalculatePhong(float3 intensity, float3 N, float3 R, float3 V, float3 L, float Ka, float Kd, float Ks, float A)
{
	float3 Ia = 0.0f;//Ka * gAmbientLightIntensity;
	float3 Id = Kd * saturate(dot(N, L));
	float3 Is = Ks * pow(saturate(dot(R, V)), A);
	
	return Ia + (Id + Is) * intensity;
}

float3 CalculateDirectionalLight(float3 intensity, float3 posW, float4 color, float3 normalW, float4 material, float3 direction)
{
	float3 L = -direction;

	float3 R = normalize(reflect(L, normalW));
	float3 V = normalize(gEyePositionW - posW);


	return color.xyz + CalculatePhong(intensity, normalW, R, V, L, material.x, material.y, material.z, material.w);
}

float3 CalculatePointLight(int index, float3 posW, float4 color, float3 normalW, float4 material)
{
	float3 direction = posW - gPointLights[index].PositionW.xyz;
	if (length(direction) <= gPointLights[index].Radius)
	{
		return CalculateDirectionalLight(gPointLights[index].Intensity, posW, color, normalW, material, direction);	
	}
	return float3(1.0f, 1.0f, 1.0f);
}
*/

float2 Fa(float dotp)
{
	if (dotp > 0)
	{
		return float2(dotp, 1);
	}
	else
	{
		return float2(0, 0);
	}
}

float Fb(float dotp, float shininess)
{
	if(dotp > 0)
	{
		return pow(abs(dotp), shininess);
	}
	else
	{
		return 0; // kill the lighting
	}
}


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
	float4 sampledColor = gColorBuffer.Sample(LinearSampler, input.TexCoord);
	float4 sampledPosition = gPositionBuffer.Sample(LinearSampler, input.TexCoord);
	float4 sampledNormal = gNormalBuffer.Sample(LinearSampler, input.TexCoord);
	float4 sampledMaterial = gMaterialBuffer.Sample(LinearSampler, input.TexCoord);

	float4 albedo = float4(sampledColor.xyz, 1.0f);
	float3 posW = sampledPosition.xyz;
	float3 normalW = normalize(sampledNormal.xyz);

	float4 color = 0;
	float2 Diffuse = float2(0, 0);
	float Specular = 0;
	
	Diffuse = Fa(dot(normalW, -gDirectionalLight.DirectionW.xyz));
	Specular = Fb(dot(normalW, normalize(normalW - gDirectionalLight.DirectionW.xyz)), sampledMaterial.w);
	color += albedo * Diffuse.x * gDirectionalLight.Intensity * sampledMaterial.y;
	color += Specular * gDirectionalLight.Intensity * Diffuse.y * sampledMaterial.z;
	
	for (int i = 0; i < gPointLightCount; ++i)
	{
		float3 lightW = gPointLights[i].PositionW.xyz - posW;
		float factor = length(lightW) / gPointLights[i].Radius;
		if (factor <= 1.0f)
		{
			float4 intensity = float4(gPointLights[i].Intensity, 1.0f) * (1.0f - pow(factor, 4));
			Diffuse = Fa(dot(normalW, lightW));
			Specular = Fb(dot(normalW, normalize(normalW + lightW)), sampledMaterial.w);
			color += albedo * Diffuse.x * intensity * sampledMaterial.y;
			color += Specular * intensity * Diffuse.y * sampledMaterial.z;
		}
	}

	float4 glowColor = float4(sampledColor.w, sampledPosition.w, sampledNormal.w, 0.0f);
	color = (0.8f * albedo) + (0.2f * color);
	color += glowColor;

	// apply ssao
	if (gSSAOToggle)
	{
		color *= gSSAOBuffer.Sample(LinearSampler, input.TexCoord).r;
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
		SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	}
}