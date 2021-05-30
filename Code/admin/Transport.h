#pragma once

#include <Client.hpp>

struct Transport : TiltedPhoques::Client
{
    void OnConsume(const void* apData, uint32_t aSize) override;
    void OnConnected() override;
    void OnDisconnected(EDisconnectReason aReason) override;
    void OnUpdate() override;
};
