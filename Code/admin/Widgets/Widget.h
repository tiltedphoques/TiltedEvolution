#pragma once

enum class WidgetID
{
    CONSOLE,
    ADMIN,
    COUNT
};

struct AdminApp;

struct Widget
{
    virtual ~Widget() = default;

    virtual bool OnEnable() = 0;
    virtual bool OnDisable() = 0;
    virtual void Update(AdminApp& aApp) = 0;
};
