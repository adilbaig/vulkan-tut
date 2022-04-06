[[vk::binding(0, 0)]] RWStructuredBuffer<uint> InBuffer;
[[vk::binding(1, 0)]] RWStructuredBuffer<uint> OutBuffer;

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    OutBuffer[DTid.x] = InBuffer[DTid.x] * InBuffer[DTid.x];
}
