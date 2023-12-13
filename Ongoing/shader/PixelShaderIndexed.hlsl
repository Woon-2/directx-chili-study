cbuffer CBuf {
    float3 face_color[6];
};

float4 main( uint tid : SV_PrimitiveID ) : SV_Target {
    return float4( face_color[(tid / 2) % 6], 1.0f );
}