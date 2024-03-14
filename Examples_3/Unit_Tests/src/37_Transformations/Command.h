#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class Command
{
public:

    Command(Cmd*);

    void build();

private:
    Cmd* cmd;
};
