float4 main(const float3 color : Color) : SV_Target {
    return float4(color, 1.0f);
}