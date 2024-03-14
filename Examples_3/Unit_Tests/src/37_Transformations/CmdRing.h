#pragma once

#include "../../../../Common_3/Graphics/Interfaces/IGraphics.h"

class CmdRingElement;

class CmdRing
{
public:
    CmdRing();

    void remove();

    CmdRingElement getNextElement();

    private:


};