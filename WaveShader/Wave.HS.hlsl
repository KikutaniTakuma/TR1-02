#include "Wave.hlsli"


ConstantHullShaderoutPut ConstantsHS_Main( InputPatch<VertexShaderOutput, 3> ip, uint PatchID : SV_PrimitiveID )
{
  ConstantHullShaderoutPut output;

  output.Edges[0] = output.Edges[1] = output.Edges[2] = 3;
  output.Inside = 16;
 
  return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantsHS_Main")]
HullShaderoutPut main( InputPatch<VertexShaderOutput, 3> input, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID )
{
  HullShaderoutPut output;

  output.position = input[i].position;
  output.color = input[i].color;
  output.normal = input[i].normal;

  return output;
}