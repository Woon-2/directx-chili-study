cbuffer CBuf {
    float3 color : Color;
};

float4 main() : SV_Target {
    return float4(color, 1.f);
}