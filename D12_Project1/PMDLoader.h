#pragma once

#include "PMD.h"
#include "Model.h"
#include <string>

PMDData LoadPMD(const std::string& path);

ModelData ConvertPMDToModelData(const PMDData&pmdDara);