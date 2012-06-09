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
Texture2D gDepthBuffer;
Texture2D gRandomBuffer;

float3 gAmbientLightIntensity;
DirectionalLight gDirectionalLight;
PointLight gPointLights[100];
int gPointLightCount;

float3 gEyePositionW;

bool gSSAOToggle = true;
int gAOSampleCount = 8;

float gAORadius = 10.0f;
float gAOScale = 5.0f;
float gAOBias = 0.0001f;
float gAOIntensity = 10.0f;



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



float2 GetRandom(float uv)
{
	float2 random = gRandomBuffer.Sample(LinearSampler, uv).xy;
	//random = random * 2.0f - 1.0f;
	random = (random + 1.0f) * 0.5f;

	return random;
}

float GetSSAOValue(float2 uv, float2 uvOffset, float3 positionW, float3 normalW)
{
	float3 occluderPositionW = gPositionBuffer.Sample(LinearSampler, uv + uvOffset).xyz;
	float3 diff = occluderPositionW - positionW;
	const float3 v = normalize(diff);
	const float d = length(diff) * gAOScale;
	return max(0.0f, dot(normalW, v) - gAOBias) * (1.0f / (1.0f + d)) * gAOIntensity;
}

float CalculateSSAO(float3 positionW, float3 normalW, float2 uv)
{
	//float2 random = normalize(saturate(gRandomBuffer.Sample(LinearSampler, uv).xy));
	float depth = gDepthBuffer.Sample(LinearSampler, uv).x;
	//float2 random = GetRandom(uv);
	float2 random = GetRandom(normalize(normalW.xy));

	int N = 4;
	const float2 C_NEIGHBOURS[4] = { float2(1.0f, 0.0f), float2(-1.0f, 0.0f)
								   , float2(0.0f, 1.0f), float2(0.0f, -1.0f) };

	float ao = 0.0f;
	float radius = gAORadius / depth;
	for (int i = 0; i < N; ++i)
	{
		float2 coord[2] = { float2(0.0f, 0.0f), float2(0.0f, 0.0f) };
		coord[0] = reflect(C_NEIGHBOURS[i], random) * radius;
		coord[1] = float2(coord[0].x * 0.707 - coord[0].y * 0.707,
						  coord[1].x * 0.707 + coord[1].y * 0.707);

		ao += GetSSAOValue(uv, coord[0] * 0.25, positionW, normalW);
		ao += GetSSAOValue(uv, coord[1] * 0.5, positionW, normalW);
		ao += GetSSAOValue(uv, coord[0] * 0.75, positionW, normalW);
		ao += GetSSAOValue(uv, coord[1], positionW, normalW);
	}

	ao /= N * 4.0f;

	return ao;
}



/*
float3 RandomOffset(float2 uv)
{
	float3 rand = normalize(gRandomBuffer.Sample(LinearSampler, uv).xyz);
	
	return rand;
}

float OcclusionFunction(float distSquared)
{
	if (distSquared < 0.01f)
		return 0.0f;
	return 1.0f / (1.0f + distSquared);
	//return exp(-distSquared);
}


float CalculateSSAO(float3 positionW, float3 normalW, float2 uv)
{
	float3 positionV = mul(float4(positionW, 1.0f), gView).xyz;
	float3 normalV = mul(float4(normalW, 0.0f), gView).xyz;

	float4 positionH = mul(float4(positionV, 1.0f), gProjection);
	positionH /= positionH.w;

	float ao = 0.0f;
	float2 uvOffset = float2(1.0 / gAOSampleCount, 1.0 / gAOSampleCount);
	for (int i = 0; i < gAOSampleCount; ++i)
	{
		// Randomize an offset, and make sure it points out of the plane
		float3 randomOffset = RandomOffset(uv + uvOffset * i);
		if (dot(randomOffset, normalV) <= 0.0f)
			randomOffset = -randomOffset;
			
		// Store the randomized point
		float3 offset = positionV + randomOffset;
		
		// Store the distance of the randomized point from the source
		float3 offsetDistanceSquared = dot(randomOffset, randomOffset);

		// Project the randomized point back to screen space
		float4 projectionOffset = mul(float4(offset, 1.0f), gProjection);
		offset = projectionOffset.xyz / projectionOffset.w;
		//offset.xy = (offset.xy * 0.5) + 0.5f;
		

		if (offset.z > positionH.z)
		//if (gDepthBuffer.Sample(LinearSampler, projectionOffset.xy).r > gDepthBuffer.Sample(LinearSampler, uv).r)
		{
			// This randomized point occludes our source
			ao += OcclusionFunction(offsetDistanceSquared);
		}
	}

	// Average the AO samples
	ao /= gAOSampleCount;

	return ao;
	//return RandomOffset(uv).x;
}
*/

/*
float2 RandomOffset(float2 uv)
{
	//uv.x = uv.x * 256.0f / 1024.0f;
	//uv.y = uv.y * 256.0f / 768.0f;

	float radius = 0.01f;
	float2 uvOffset = normalize(gRandomBuffer.Sample(LinearSampler, uv).xy);
	uvOffset.x = uvOffset.x * radius / 1024.0f;
	uvOffset.y = uvOffset.y * radius / 768.0f;

	return uvOffset;
}
*/

/*
float3 RandomOffset(float2 uv)
{
	float3 rand = normalize(gRandomBuffer.Sample(LinearSampler, uv).xyz);
	
	return rand;
}

float CalculateSSAO(float3 positionW, float2 uv)
{
	float4 positionH = mul(float4(positionW, 1.0f), mul(gView, gProjection));
	positionH /= positionH.w;

	float sourceDepth = gDepthBuffer.Sample(LinearSampler, uv).r;
	float occlusion = 0.0f;
	for (int i = 0; i < gAOSampleCount; ++i)
	{
		float3 sampleW = positionW + RandomOffset(uv + float2(0.1, 0.1) * i);
		float4 sampleOffset = mul(float4(sampleW, 1.0f), mul(gView, gProjection));
		sampleOffset.xyz /= sampleOffset.w;
		
		float2 sampleUV = 2.0f * sampleOffset.xy - 1.0f;
		float sampleDepth = gDepthBuffer.Sample(LinearSampler, sampleUV).r;

		if (sampleDepth > 0.999)
		{
			occlusion++;
		}
		else
		{
			float depthDiff = sourceDepth - sampleDepth;
			if (depthDiff > 0)
			{
				occlusion += 1.0f - (depthDiff);
			}
		}
	}

	occlusion = occlusion / gAOSampleCount;

	return occlusion;
}*/


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

	/*
	// calculate ssao
	if (gSSAOToggle)
	{
		float ao = CalculateSSAO(posW, normalW, input.TexCoord);
		//color *= 1.0f - ao;
		//color *= ao;
		//ao = 1.0f - ao;

		return float4(ao, ao, ao, 1.0f);
	}
	*/
	
	return color;

	/*
	float4 result = float4(CalculateDirectionalLight(gDirectionalLight.Intensity.xyz, posW, albedo, normalW, material, gDirectionalLight.DirectionW.xyz), 0.0f);
	for (int i = 0; i < gPointLightCount; ++i)
	{
		result += float4(CalculatePointLight(i, posW, albedo, normalW, material), 0.0f);
	}

	return result;
	*/
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	//return C * saturate(dot(N, L)) * float4(gLightIntensity, 1.0f);
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