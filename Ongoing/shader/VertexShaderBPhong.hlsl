// separte cbuffers temporarily.
// integrate them later when the Transform Constant Buffer class is well defined.

cbuffer TransCBuf1 {
    matrix worldView;
};

cbuffer TransCBuf2 {
    matrix worldViewProj;
};

struct VSOut {
    float3 worldPos : Position;
    float3 normal : Normal;
    float4 pos : SV_Position;
};

VSOut main( float3 pos : Position, float3 n : Normal )
{
    VSOut vso;
    vso.worldPos = (float3)mul( float4(pos, 1.0f), worldView );
    vso.normal = mul( n, (float3x3)worldView );
    vso.pos = mul( float4(pos, 1.0f), worldViewProj );
    return vso;
}