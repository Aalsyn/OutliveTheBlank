// La premiere partie du code si dessous implemente un gradient de perlin dans le but d'avoir de la variation sur le shading de l'herbe ! je ne suis pas l'auteur de ce code !
float interpolate(float a0, float a1, float w) {
	/* // You may want clamping by inserting:
	 * if (0.0 > w) return a0;
	 * if (1.0 < w) return a1;
	 */
	return (a1 - a0) * w + a0;
	/* // Use this cubic interpolation [[Smoothstep]] instead, for a smooth appearance:
	 * return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
	 *
	 * // Use [[Smootherstep]] for an even smoother result with a second derivative equal to zero on boundaries:
	 * return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
	 */
}

typedef struct {
	float x, y;
} vector2;

/* Create pseudorandom direction vector
 */
vector2 randomGradient(int ix, int iy) {
	// No precomputed gradients mean this works for any number of grid coordinates
	const unsigned int w = 8;
	const unsigned int s = w / 2; // rotation width
	unsigned int a = ix, b = iy;
	a *= 3284157443; b ^= a << s | a >> w - s;
	b *= 1911520717; a ^= b << s | b >> w - s;
	a *= 2048419325;
	float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
	vector2 v;
	v.x = cos(random); v.y = sin(random);
	return v;
}

float dotGridGradient(int ix, int iy, float x, float y) {
	// Get gradient from integer coordinates
	vector2 gradient = randomGradient(ix, iy);

	// Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;

	// Compute the dot-product
	return (dx * gradient.x + dy * gradient.y);
}

float perlin(float x, float y) {
	// Determine grid cell coordinates
	int x0 = (int)floor(x);
	int x1 = x0 + 1;
	int y0 = (int)floor(y);
	int y1 = y0 + 1;

	// Determine interpolation weights
	// Could also use higher order polynomial/s-curve here
	float sx = x - (float)x0;
	float sy = y - (float)y0;

	// Interpolate between grid point gradients
	float n0, n1, ix0, ix1, value;

	n0 = dotGridGradient(x0, y0, x, y);
	n1 = dotGridGradient(x1, y0, x, y);
	ix0 = interpolate(n0, n1, sx);

	n0 = dotGridGradient(x0, y1, x, y);
	n1 = dotGridGradient(x1, y1, x, y);
	ix1 = interpolate(n0, n1, sx);

	value = interpolate(ix0, ix1, sy);
	return value; // Will return in range -1 to 1. To make it in range 0 to 1, multiply by 0.5 and add 0.5
}

#define LIGHT_NUMBER 2
#define DIMTEX 16384

cbuffer LightParams
{
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

cbuffer CameraParams
{
    float4x4 matWorldViewProj; // la matrice totale 
    float4x4 matWorld; // matrice de transformation dans le monde 
    float4 cPosition; // la position de la cam�ra
}

cbuffer MaterialParams
{
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
    float3 Norm : NORMAL;
    float3 vDirCam : TEXCOORD0;
    float2 coordTex : TEXCOORD1;
    float Noise : TEXTCOORD2;
    float3 vDirLum[LIGHT_NUMBER] : TEXCOORD3;
    float4 posInMap[LIGHT_NUMBER] : TEXCOORD9; //only works if LIGHT_NUMBER <= 6
};


VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
    VS_Sortie sortie = (VS_Sortie) 0;

    sortie.Pos = mul(Pos, matWorldViewProj);
    sortie.Norm = normalize(mul(float4(Normale, 0.0f), matWorld).xyz);
    float3 PosWorld = mul(Pos, matWorld).xyz;
    sortie.Noise = perlin(PosWorld.x / 5, PosWorld.z / 5);
	
    for (uint i = 0; i != LIGHT_NUMBER; ++i)
    {
        if (isPoint[i].x > 0.1f) //temporary, should be isPoint[i] > 0
        {
            sortie.vDirLum[i] = lPosition[i].xyz - PosWorld;
        }
        else
        {
            sortie.vDirLum[i] = -lDirection[i].xyz;
        }
        sortie.posInMap[i] = mul(Pos, lMatWVP[i]);
    }
    sortie.vDirCam = normalize(cPosition.xyz - PosWorld);

	// Coordonn�es d'application de texture
    sortie.coordTex = coordTex;

    return sortie;
}

Texture2D textureEntree; // la texture
Texture2D shadowTexture; //shadow map
SamplerState SampleState; // sampling texture de base
SamplerState SampleStyleClamp //sampling shadow map
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

Texture2D texture_Cliff; // la texture des fassade de terrain
Texture2D texture_Grass; // la texture de autre chose
Texture2D texture_Water; // la texture de autre chose




float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
    float3 finalColor = 0;
	// Normaliser les param�tres
    float3 N = vs.Norm;
    float3 V = vs.vDirCam;
    float4 ambiantTexColor = 0;
    float4 diffuseTexColor = 0;
    if (1 > 0) //should be texIsActive > 0 but it doesn't work for some reason
    {
		// echantillonner la couleur du pixel � partir de la texture
        ambiantTexColor = textureEntree.Sample(SampleState, vs.coordTex);
        // cliff blend (we use a continuious unit step function as an alpha mask)
        float blendAlphaCliff = (0.5) * (tanh((vs.Norm.y) * 35 - 33)) + 0.5;
        ambiantTexColor = ambiantTexColor * blendAlphaCliff + texture_Cliff.Sample(SampleState, vs.coordTex).rgb * (1.0 - blendAlphaCliff);
	
	    // grass blend (we use xz normal component because y is not precise enought (we use a continuious unit step function as an alpha mask)
        float blendAlphaGrass = (0.5) * tanh(sqrt(vs.Norm.x * vs.Norm.x + vs.Norm.z * vs.Norm.z) * (-70) + 6) + 0.5;
        ambiantTexColor = texture_Grass.Sample(SampleState, vs.coordTex).rgb * blendAlphaGrass + ambiantTexColor * (1.0 - blendAlphaGrass);
    
	    // add perlin noise to textures to have varring shade of the terrain
        ambiantTexColor += ambiantTexColor * (vs.Noise) * 0.1;
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
            finalColor += ambiantTexColor.rgb * lAmbiant[i].rgb * 0.5f;
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
        float3 L = normalize(vs.vDirLum[i]);
		// Valeur de la composante diffuse
        float3 diff = saturate(dot(N, L));
        float3 revdiff = saturate(dot(-N, L));

		// R = 2 * (N.L) * N - L
        float3 R = normalize(2 * diff * N - L);
 
		// Calcul de la sp�cularit� 
        float3 S = pow(saturate(dot(R, V)), specAttenuation.x);

        float ValeurOmbre = 1.0f;
        //si lumiere directionnelle, calcul d'ombre
        if (isPoint[i].x < 0.1f)
        {
		    // Calculate the projected texture coordinates.
            ShadowCoord = 0.5f * vs.posInMap[i].xy / vs.posInMap[i].w + float2(0.5f, 0.5f);
            ShadowCoord.y = 1.0f - ShadowCoord.y;
 
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
        
        finalColor += (ambiantTexColor.rgb * lAmbiant[i].rgb + (diffuseTexColor.rgb * lDiffuse[i].rgb * diff +
					lSpecular[i].rgb * mSpecular.rgb * S) * ValeurOmbre) * fatt;             
    }
    finalColor = saturate(finalColor);
    //finalColor = ambiantTexColor;
    return float4(finalColor, 1.0f);
}

technique11 MiniPhong
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, MiniPhongVS()));
		SetPixelShader(CompileShader(ps_5_0, MiniPhongPS()));
		SetGeometryShader(NULL);
	}
}
