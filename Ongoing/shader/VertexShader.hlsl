struct VSOut {
    float4 color : Color;
    float4 pos : SV_Position;
};

cbuffer CBuf {
    matrix transform;
};

VSOut main(float2 pos : Position, float4 color : Color) {
    VSOut vso;
    vso.pos = mul(float4(pos.x, pos.y, 0.f, 1.f), transform);
    vso.color = color;

    return vso;
}