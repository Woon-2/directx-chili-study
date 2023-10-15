struct VSOut
{
    float4 color : Color;
    float4 pos : SV_Position;
};

VSOut main(float2 pos : Position, float4 color : Color)
{
    VSOut vso;
    vso.pos = float4(pos.x, pos.y, 0.f, 1.f);
    vso.color = color;

    return vso;
}