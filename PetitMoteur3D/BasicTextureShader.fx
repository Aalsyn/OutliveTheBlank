//cbuffer param
//{ 
//	float4x4 matWorldViewProj;   // la matrice totale 
//	float4x4 matWorld;		// matrice de transformation dans le monde 
//	float4 vLumiere; 		// la position de la source d'�clairage (Point)
//	float4 vCamera; 			// la position de la cam�ra
//	float4 vAEcl; 			// la valeur ambiante de l'�clairage
//	float4 vAMat; 			// la valeur ambiante du mat�riau
//	float4 vDEcl; 			// la valeur diffuse de l'�clairage 
//	float4 vDMat; 			// la valeur diffuse du mat�riau 
//	float4 vSEcl; 			// la valeur sp�culaire de l'�clairage 
//	float4 vSMat; 			// la valeur sp�culaire du mat�riau 
//	float puissance;
//	int bTex;		    // Bool�en pour la pr�sence de texture
//	float2 remplissage;
//}
#define LIGHT_NUMBER 1
//cbuffer lightNumber{
//	int lightNumber;
//}

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
	float2 filling;
	float specAttenuation;
	int texIsActive;
}


struct VS_Sortie
{
	float4 Pos : SV_Position;
	float3 Norm :    NORMAL;
    float3 vDirCam : TEXCOORD0;
	float2 coordTex : TEXCOORD1; 
	float3 vDirLum[LIGHT_NUMBER] : TEXCOORD2;
};

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex: TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = mul(Pos, matWorldViewProj);
	sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

	float3 PosWorld = mul(Pos, matWorld).xyz;

    for (uint i = 0; i != LIGHT_NUMBER; ++i)
    {	
		if (isPoint[i].x > 0) //temporary, should be isPoint[i] > 0
		{
			sortie.vDirLum[i] = lPosition[i].xyz - PosWorld;
		}
		else
		{
			sortie.vDirLum[i] = -lDirection[i].xyz;
		}
    }
	sortie.vDirCam = cPosition.xyz - PosWorld;

	// Coordonn�es d'application de texture
    sortie.coordTex = uvScaling.rg * coordTex;

	return sortie;
}

Texture2D textureEntree;  // la texture

SamplerState SampleState;  // l'�tat de sampling

float4 MiniPhongPS( VS_Sortie vs ) : SV_Target
{
    float3 finalColor = 0;
	// Normaliser les param�tres
	float3 N = normalize(vs.Norm);
	float3 V = normalize(vs.vDirCam);
	float4 ambiantTexColor = 0;
    float4 diffuseTexColor = 0;
    if (1 > 0) //should be texIsActive > 0 but it doesn't work for some reason
    {
		// echantillonner la couleur du pixel � partir de la texture
        ambiantTexColor = textureEntree.Sample(SampleState, vs.coordTex);
        diffuseTexColor = ambiantTexColor;
    }
	else
    {
        ambiantTexColor = mAmbiant;
        diffuseTexColor = mDiffuse;
    }
	
    for (uint i = 0; i != LIGHT_NUMBER; ++i)
    {
        float distanceFromLight = sqrt(pow(vs.vDirLum[i].x, 2.0f) + pow(vs.vDirLum[i].y, 2.0f) + pow(vs.vDirLum[i].z, 2.0f));
		
		// lAttenuationParam[]
		// 0 light power
		// 1 light min distance
		// 2 light max distance
		// 3 should atenuate or not
        float fatt;
        if (lAttenuationParam[i].w < 0.1){fatt = 1;} // check if not 0 (can only be 0 or 1 we used 0.1 to account for error)
		else if (distanceFromLight < lAttenuationParam[i].y){ fatt = 1;}
        else if (distanceFromLight > lAttenuationParam[i].z){ fatt = 0;}
        else { fatt = lAttenuationParam[i].x * saturate(1 / distanceFromLight);}
		
        //float atten = 1.0 - saturate(length(vs.vDirLum[i]) * lAttenuation[i]);
        //atten *= atten;
		float3 L = normalize(vs.vDirLum[i]);
		// Valeur de la composante diffuse
		float3 diff = saturate(dot(N, L));
        float3 revdiff = saturate(dot(-N, L));

	// R = 2 * (N.L) * N � L
        float3 R = normalize(2 * diff * N - L);
 
	// Calcul de la sp�cularit� 
        float3 S = pow(saturate(dot(R, V)), 4); //temporary, should be power of specAttenuation

        finalColor += (ambiantTexColor.rgb *lAmbiant[i].rgb +
					fatt * diffuseTexColor.rgb * lDiffuse[i].rgb * diff +
					lSpecular[i].rgb * mSpecular.rgb * S);

    }
    finalColor = saturate(finalColor);
    //finalColor = ambiantTexColor;
    return float4(finalColor, 1.0f);
}


////////TEST render texture
Texture2D shadowTexture; //shadow map
SamplerState SampleStyleClamp //sampling shadow map
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

float4 RenderTextureTestPS(VS_Sortie vs) : SV_Target
{
    return shadowTexture.Sample(SampleStyleClamp, vs.coordTex); //test that the shadowmap texture is correctly passed
}

technique11 MiniPhong
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_4_0, MiniPhongVS()));
		SetPixelShader(CompileShader(ps_4_0, MiniPhongPS()));
        //SetPixelShader(CompileShader(ps_4_0, RenderTextureTestPS()));
		SetGeometryShader(NULL);
	}
}
