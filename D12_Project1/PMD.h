#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>

using namespace DirectX;

#pragma pack(push, 1)
struct PMD_VERTEX
{
    XMFLOAT3 pos;
    XMFLOAT3 normal;
    XMFLOAT2 uv;
    uint16_t bone_no[2];
    uint8_t  weight;
    uint8_t  EdgeFlag;
    uint16_t dummy;
};
#pragma pack(pop)

struct PMDHeader {
    float version; //例：00 00 80 3F == 1.00
    char model_name[20];//モデル名
    char comment[256];//モデルコメント
};

#pragma pack(push, 1)
struct PMDBone {
    char name[20];

    uint16_t parentBoneIndex;
    uint16_t tailBoneIndex;

    uint8_t boneType;

    uint16_t ikParentBoneIndex;

    DirectX::XMFLOAT3 position;
};

#pragma pack(1)
//PMDマテリアル構造体
struct PMDMaterial {
    XMFLOAT3 diffuse; //ディフューズ色
    float alpha; // ディフューズα
    float specularity;//スペキュラの強さ(乗算値)
    XMFLOAT3 specular; //スペキュラ色
    XMFLOAT3 ambient; //アンビエント色
    unsigned char toonIdx; //トゥーン番号(後述)
    unsigned char edgeFlg;//マテリアル毎の輪郭線フラグ
    //2バイトのパディングが発生！！
    unsigned int indicesNum; //このマテリアルが割り当たるインデックス数
    char texFilePath[20]; //テクスチャファイル名(プラスアルファ…後述)
};//70バイトのはず…でもパディングが発生するため72バイト
#pragma pack()//1バイトパッキング解除

//シェーダ側に投げられるマテリアルデータ
struct MaterialForHlsl {
    XMFLOAT3 diffuse; //ディフューズ色
    float alpha; // ディフューズα
    XMFLOAT3 specular; //スペキュラ色
    float specularity;//スペキュラの強さ(乗算値)
    XMFLOAT3 ambient; //アンビエント色
};

//それ以外のマテリアルデータ
struct AdditionalMaterial {
    std::string texPath;//テクスチャファイルパス
    int toonIdx; //トゥーン番号
    bool edgeFlg;//マテリアル毎の輪郭線フラグ
};

//まとめたもの
struct Material {
    unsigned int indicesNum;//インデックス数
    MaterialForHlsl material;
    AdditionalMaterial additional;
};

//PMDのデータをまとめたもの
struct PMDData
{
    PMDHeader header;
    std::vector<PMD_VERTEX> vertices;
    std::vector<unsigned short> indices;
    std::vector<Material> materials;

    std::vector<PMDBone> bones;
};