#pragma once

#include <string>
#include <vector>
#include <map>

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

struct shaderCreationParameters
{
    std::string              shaderName;
    std::vector<std::string> shaderStageFileNames;
};

class Signature
{
public:
    Signature(std::vector<std::string> samplerNames, std::vector<shaderCreationParameters> shaderParams);

    ~Signature();

    RootSignature* getRootSignature();

    Shader* getShaderByName(std::string shaderName);

private:
    RootSignature* rootSignature;
    std::map<std::string, Shader*> shaders;
    std::map<std::string, Sampler*> samplers;
};
