#define LIGHT_NUMBER 2
#define DIMTEX 16384

cbuffer LightParams {
	float4 lAmbiant[LIGHT_NUMBER];
	float4 lDiffuse[LIGHT_NUMBER];
	float4 lSpecular[LIGHT_NUMBER];
	float4 lPosition[LIGHT_NUMBER]; // used for point light only
	float4 lDirection[LIGHT_NUMBER]; // used for directionnal light only
    float4x4 lMatWVP[LIGHT_NUMBER];
    float4 lAttenuationParam[LIGHT_NUMBER];
    float4 lIsActive[LIGHT_NUMBER]; //currently unused (doesn't work for some reason)
    float4 isPoint[LIGHT_NUMBER]; //if false, the light is directionnal (temporary, should be changed if spotlight is added)
    //float2 padding[LIGHT_NUMBER];
}

cbuffer CameraParams {
	float4x4 matWorldViewProj;	// la matrice totale 
	float4x4 matWorld;			// matrice de transformation dans le monde 
	float4 cPosition; 			// la position de la cam�ra
}

cbuffer MaterialParams {
	float4 mAmbiant;
	float4 mDiffuse;
    float4 mSpecular;
	float4 uvScaling;
	float4 specAttenuation;
	float4 texIsActive;
}



struct VS_Sortie
{
	float4 Pos : SV_Position;
	float4 Norm :    NORMAL;
	float4 vDirCam : TEXCOORD0;
	float2 coordTex : TEXCOORD1; 
	float4 vDirLum[LIGHT_NUMBER] : TEXCOORD2;
    float4 posInMap[LIGHT_NUMBER] : TEXCOORD9; //only works if LIGHT_NUMBER <= 7
};

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
    VS_Sortie sortie = (VS_Sortie) 0;

    sortie.Pos = mul(Pos, matWorldViewProj);
    sortie.Norm = normalize(mul(float4(Normale, 0.0f), matWorld));

	// Coordonn�es d'application de texture
    sortie.coordTex = coordTex;

    return sortie;
}

Texture2D textureEntree;  // la texture
Texture2D shadowTexture; //shadow map
SamplerState SampleState;  // sampling texture de base

float4 MiniPhongPS( VS_Sortie vs ) : SV_Target
{
    return textureEntree.Sample(SampleState, vs.coordTex);
    
}

technique11 MiniPhong
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_4_0, MiniPhongVS()));
		SetPixelShader(CompileShader(ps_4_0, MiniPhongPS()));
		SetGeometryShader(NULL);
	}
}
