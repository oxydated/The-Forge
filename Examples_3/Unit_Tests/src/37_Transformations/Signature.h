#pragma once

#include <string>
#include <vector>

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class Signature
{
public:
    Signature(std::vector<std::string> samplerNames, std::vector<std::string> shaderFileNames);

    RootSignature* getRootSignature();

private:
    RootSignature* rootSignature;
};
