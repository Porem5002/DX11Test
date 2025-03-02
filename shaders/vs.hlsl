struct VSOut
{
    float4 color : Color;
    float4 pos : SV_Position;
};

VSOut main(float2 pos : Position, float4 color : Color)
{
    VSOut vs_out;
    vs_out.pos = float4(pos, 0.0f, 1.0f);
    vs_out.color = color;
    return vs_out;
}