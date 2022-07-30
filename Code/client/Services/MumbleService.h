#pragma once

struct TransportService;
struct UpdateEvent;
struct LinkedMem;

class MumbleService final : public entt::registry
{
public:
    MumbleService(entt::dispatcher&, TransportService& aTransport);
    ~MumbleService();

    void Update(const UpdateEvent& acEvent);

private:

    LinkedMem* m_pLinkedMem;
    TiltedPhoques::WString m_uniqueIdentifier;
    TransportService& m_transport;
};
