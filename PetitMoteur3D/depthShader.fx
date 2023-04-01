cbuffer PovParams
{
    float4x4 matWorldViewProj; // la matrice totale
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

struct VS_Output
{
    float4 Pos : SV_Position;
    //float4 depthPos : TEXCOORD0;
};

VS_Output DepthVS(float4 Pos : POSITION)
{
    //Pos.w = 1.0f;
    VS_Output vs_out = (VS_Output) 0;
    vs_out.Pos = mul(Pos, matWorldViewProj);
    
    return vs_out;
}

float4 DepthPS(VS_Output input) : SV_Target
{
    float depthValue;
    depthValue = input.Pos.z / input.Pos.w;
    return float4(depthValue, 0.0f, 0.0f, 1.0f);
}

/*
RasterizerState rsCullFront
{
    CullMode = Front;
};

RasterizerState rsCullBack
{
    CullMode = Back;
};
*/
//-------------------------------------------------------------------
// Technique pour le shadow map
//-------------------------------------------------------------------
technique11 Depth
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_4_0, DepthVS()));
        SetPixelShader(CompileShader(ps_4_0, DepthPS()));
        //SetPixelShader(NULL);
        SetGeometryShader(NULL);

    }
}