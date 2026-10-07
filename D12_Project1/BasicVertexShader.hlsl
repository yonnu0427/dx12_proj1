#include"BasicType.hlsli"

//定数バッファ0
cbuffer ObjectData : register(b0)
{
    matrix world; //ワールド変換行列
};

cbuffer CameraData : register(b1)
{
    matrix view;
    matrix proj; //ビュープロジェクション行列
    float3 eye;
};

StructuredBuffer<float4x4> skinMatrices : register(t4);

//頂点シェーダ→ピクセルシェーダへのやり取りに使用する
//構造体
struct Output
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 pos : POSITION; //システム用頂点座標
    float4 normal : NORMAL0; //法線ベクトル
    float4 vnormal : NORMAL1; //法線ベクトル
    float2 uv : TEXCOORD; //UV値
    float3 ray : VECTOR; //ベクトル
};

BasicType BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, uint4 boneIndices : BONE_INDEX, float4 boneWeights : BONE_WEIGHT)
{
    BasicType output; //ピクセルシェーダへ渡す値
    
    float4 skinnedPos = float4(0, 0, 0, 0);
    float3 skinnedNormal = float3(0, 0, 0);
    
    if (dot(boneWeights, float4(1, 1, 1, 1)) > 0.0f)
    {
        [unroll]
        for (int i = 0; i < 4; ++i)
        {
            if (boneWeights[i] > 0.0f)
            {
                skinnedPos += mul(skinMatrices[boneIndices[i]], pos) * boneWeights[i];
                skinnedNormal += mul((float3x3)skinMatrices[boneIndices[i]], normal.xyz) * boneWeights[i];

            }
        }
        pos = skinnedPos;
        normal.xyz = normalize(skinnedNormal);

    }
    
    pos = mul(world, pos);
    
    output.svpos = mul(mul(proj, view), pos); //シェーダでは列優先なので注意
    output.pos = mul(view, pos);
    normal.w = 0; //ここ重要(平行移動成分を無効にする)
    output.normal = mul(world, normal); //法線にもワールド変換を行う
    output.vnormal = mul(view, output.normal);
    output.uv = uv;
    output.ray = normalize(pos.xyz - mul(view, eye)); //視線ベクトル

    return output;
}