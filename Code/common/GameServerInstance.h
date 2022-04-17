#pragma once

class IGameServerInstance
{
  public:
    virtual ~IGameServerInstance() = default;

    // lifetime control
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    virtual bool IsListening() = 0;
    virtual bool IsRunning() = 0;

    // update the server logic
    virtual void Update() = 0;
};
