#pragma once

#include "../../../../Common_3/Application/Interfaces/IApp.h"

class Command;

class IAppCommand: public IApp
{
protected:
    virtual void commandsToRecord(void*) = 0;
};
