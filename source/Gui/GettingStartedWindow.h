#pragma once

#include "EngineInterface/Definitions.h"

#include "Definitions.h"
#include "AlienWindow.h"

class _GettingStartedWindow : public _AlienWindow
{
public:
    _GettingStartedWindow();

    virtual ~_GettingStartedWindow();

private:
    void processIntern() override;

    bool _showAfterStartup = true;
};