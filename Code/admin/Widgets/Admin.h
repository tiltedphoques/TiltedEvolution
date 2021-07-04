#pragma once

#include "Widget.h"

struct Admin : Widget
{
    Admin();
    ~Admin() override = default;

    bool OnEnable() override;
    bool OnDisable() override;
    void Update(AdminApp& aApp) override;

private:

};
