#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <DirectXTex.h>

#include <string>
#include <utility>
#include <functional>
#include <map>

using namespace DirectX;
using namespace std;

std::string GetTexturePathFromModelAndTexPath(
    const std::string& modelPath,
    const char* texPath
);

string GetExtension(const std::string& path);

wstring GetExtension(const std::wstring& path);

pair<string, string>
SplitFileName(
    const std::string& path,
    const char splitter = '*'
);

std::wstring
GetWideStringFromString(const std::string& str);

ID3D12Resource* CreateGrayGradationTexture();

ID3D12Resource* CreateWhiteTexture();

ID3D12Resource* CreateBlackTexture();

using LoadLambda_t =
function<HRESULT(
    const wstring& path,
    TexMetadata*,
    ScratchImage&
)>;

extern map<string, LoadLambda_t> loadLambdaTable;

extern map<string, ID3D12Resource*> _resourceTable;

ID3D12Resource*
LoadTextureFromFile(std::string& texPath);