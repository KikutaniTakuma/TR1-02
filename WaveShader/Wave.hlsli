struct VertexShaderOutput{
    float32_t4 position : POSITION0;
    float32_t4 color : COLOR;
    float32_t4 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
};

struct ConstantHullShaderoutPut
{
  float Edges[3] : SV_TessFactor;
  float Inside : SV_InsideTessFactor;
};

struct HullShaderoutPut{
    float32_t4 position : SV_POSITION;
    float32_t4 color : COLOR;
    float32_t4 normal : NORMAL;
};

struct DSoutPut{
    float32_t4 position : POSITION0;
    float32_t4 color : COLOR;
    float32_t4 normal : NORMAL;
};

struct GeometoryOutPut{
    float32_t4 position : SV_POSITION;
    float32_t4 color : COLOR;
    float32_t4 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
};