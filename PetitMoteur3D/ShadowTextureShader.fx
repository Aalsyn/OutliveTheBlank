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
    float4 PosWorld = mul(Pos, matWorld);

    for (uint i = 0; i != LIGHT_NUMBER; ++i)
    {
        if (isPoint[i].x > 0.1f) //temporary, should be isPoint[i] > 0
        {
            sortie.vDirLum[i] = lPosition[i]- PosWorld;
        }
        else
        {
            sortie.vDirLum[i] = -lDirection[i];
        }
        sortie.posInMap[i] = mul(Pos, lMatWVP[i]); 
    }
    sortie.vDirCam = normalize(cPosition - PosWorld);

	// Coordonn�es d'application de texture
    sortie.coordTex = coordTex;

    return sortie;
}

Texture2D textureEntree;  // la texture
Texture2D shadowTexture; //shadow map
SamplerState SampleState;  // sampling texture de base
SamplerState SampleStyleClamp //sampling shadow map
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

float4 MiniPhongPS( VS_Sortie vs ) : SV_Target
{
    float4 finalColor = 0;
	// Normaliser les param�tres
    float4 N = vs.Norm;
    float4 V = vs.vDirCam;
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
    
	//local variables for shadow mapping
    float2 ShadowCoord;
    float depthValue;
    float lightDepthValue;
    float bias = 0.00000007f;

    //int actualLightNumber = min(lightNumber, LIGHT_NUMBER);
    for (uint i = 0; i != LIGHT_NUMBER; ++i)
    {
        //if the light is not active, apply a reduced ambiant light (in order to see something)
        if (lIsActive[i].x < 0.1f)
        {
            finalColor += ambiantTexColor * lAmbiant[i] * 0.5f;
            continue;
        }
        //ATTENUATION
		// lAttenuationParam[]
		// 0 light power
		// 1 light min distance
		// 2 light max distance
		// 3 should atenuate or not
        float fatt = 1.0f;
        if (lAttenuationParam[i].w > 0.1f)
        {
            float distanceFromLight = sqrt(pow(vs.vDirLum[i].x, 2.0f) + pow(vs.vDirLum[i].y, 2.0f) + pow(vs.vDirLum[i].z, 2.0f));		
        
            // check if not 0 (can only be 0 or 1 we used 0.1 to account for error)
            if (distanceFromLight < lAttenuationParam[i].y)
            {
                fatt = 1;
            }
            else if (distanceFromLight > lAttenuationParam[i].z)
            {
                fatt = 0;
            }
            else
            {
                fatt = lAttenuationParam[i].x * saturate(1 / distanceFromLight);
            }                    
        }
		
        //reflexion diffuse et speculaire
        float4 L = normalize(vs.vDirLum[i]);
		// Valeur de la composante diffuse
        float4 diff = saturate(dot(N, L));
        float4 revdiff = saturate(dot(-N, L));

		// R = 2 * (N.L) * N - L
		float4 R = normalize(2 * diff * N - L);
 
		// Calcul de la sp�cularit� 
		float4 S = pow(saturate(dot(R, V)), specAttenuation.x);

        float ValeurOmbre = 1.0f;
        //si lumiere directionnelle, calcul d'ombre
        if (diff.x - 0.02f <= 0.0f)
        {
            ValeurOmbre = 0.0f;
        }
        else if (isPoint[i].x < 0.1f)
        {          
		    // Calculate the projected texture coordinates.
            ShadowCoord = 0.5f * vs.posInMap[i].xy / vs.posInMap[i].w + float2(0.5f, 0.5f);
            ShadowCoord.y = 1.0f - ShadowCoord.y;
 
            if ((saturate(ShadowCoord.x) == ShadowCoord.x) && (saturate(ShadowCoord.y) == ShadowCoord.y))
            {
                float2 PixelActuel = DIMTEX * ShadowCoord; // Pour une texture de 512 X 512

	            // Valeur de l'interpolation linéaire
                float2 lerps = frac(PixelActuel);

	            // Lire les valeurs du tableau, avec les vérifications de profondeur
                float Profondeur = vs.posInMap[i].z / vs.posInMap[i].w;
                Profondeur -= bias;
                float3 kernel[9];

                float echelle = 1.0 / DIMTEX;

                float2 coord[9];
                coord[0] = ShadowCoord + float2(-echelle, -echelle);
                coord[1] = ShadowCoord + float2(-echelle, 0.0);
                coord[2] = ShadowCoord + float2(-echelle, echelle);
                coord[3] = ShadowCoord + float2(0.0, -echelle);
                coord[4] = ShadowCoord + float2(0.0, 0.0);
                coord[5] = ShadowCoord + float2(0.0, echelle);
                coord[6] = ShadowCoord + float2(echelle, -echelle);
                coord[7] = ShadowCoord + float2(echelle, 0.0);
                coord[8] = ShadowCoord + float2(echelle, echelle);

	            // Colonne 1
                kernel[0].x = (shadowTexture.Sample(SampleStyleClamp, coord[0]).r < Profondeur) ? 0.0f : 1.0f;
                kernel[0].y = (shadowTexture.Sample(SampleStyleClamp, coord[1]).r < Profondeur) ? 0.0f : 1.0f;
                kernel[0].z = (shadowTexture.Sample(SampleStyleClamp, coord[2]).r < Profondeur) ? 0.0f : 1.0f;
	            // Colonne 2
                kernel[1].x = (shadowTexture.Sample(SampleStyleClamp, coord[3]).r < Profondeur) ? 0.0f : 1.0f;
                kernel[1].y = (shadowTexture.Sample(SampleStyleClamp, coord[4]).r < Profondeur) ? 0.0f : 1.0f;
                kernel[1].z = (shadowTexture.Sample(SampleStyleClamp, coord[5]).r < Profondeur) ? 0.0f : 1.0f;
	            // Colonne 3
                kernel[2].x = (shadowTexture.Sample(SampleStyleClamp, coord[6]).r < Profondeur) ? 0.0f : 1.0f;
                kernel[2].y = (shadowTexture.Sample(SampleStyleClamp, coord[7]).r < Profondeur) ? 0.0f : 1.0f;
                kernel[2].z = (shadowTexture.Sample(SampleStyleClamp, coord[8]).r < Profondeur) ? 0.0f : 1.0f;
 
 	            // Les interpolations linéaires
	            // Interpoler colonnes 1 et 2
                float3 col12 = lerp(kernel[0], kernel[1], lerps.x);
	            // Interpoler colonnes 2 et 3
                float3 col23 = lerp(kernel[1], kernel[2], lerps.x);
 	            // Interpoler ligne 1 et colonne 1
                float4 lc;
                lc.x = lerp(col12.x, col12.y, lerps.y);
 	            // Interpoler ligne 2 et colonne 1
                lc.y = lerp(col12.y, col12.z, lerps.y);
 	            // Interpoler ligne 1 et colonne 2
                lc.z = lerp(col23.x, col23.y, lerps.y);
	            // Interpoler ligne 2 et colonne 1
                lc.w = lerp(col23.y, col23.z, lerps.y);

	            // Faire la moyenne
                ValeurOmbre = (lc.x + lc.y + lc.z + lc.w) / 4.0;
            }
        }
        
        finalColor += (ambiantTexColor * lAmbiant[i]+ (diffuseTexColor * lDiffuse[i] * diff +
					lSpecular[i]* mSpecular * S) * ValeurOmbre) * fatt;
        
        /*
		 // Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
        if ((saturate(ShadowCoord.x) == ShadowCoord.x) && (saturate(ShadowCoord.y) == ShadowCoord.y))  
        {
            // Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
                depthValue = shadowTexture.Sample(SampleStyleClamp, ShadowCoord).r;

            // Calculate the depth of the light.
                lightDepthValue =  vs.posInMap[i].z / vs.posInMap[i].w;

            // Subtract the bias from the lightDepthValue.
                lightDepthValue = lightDepthValue - bias;

            // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
            // If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
            if (lightDepthValue < depthValue)
            {
                finalColor += ( diffuseTexColor.rgb * lDiffuse[i].rgb * diff +
					    lSpecular[i].rgb * mSpecular.rgb * S) * fatt;
            }
        }
        */
        
        
        
    }
    finalColor = saturate(finalColor);
    //finalColor = ambiantTexColor;
    return finalColor;
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
