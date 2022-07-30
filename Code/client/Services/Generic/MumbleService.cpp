#include <TiltedOnlinePCH.h>

#include "Services/MumbleService.h"

#include "Events/UpdateEvent.h"

#include "Games/Skyrim/PlayerCharacter.h"
#include "Games/Skyrim/Camera/PlayerCamera.h"

#include "Services/TransportService.h"

struct LinkedMem
{
#ifdef _WIN32
    UINT32 uiVersion;
    DWORD uiTick;
#else
    uint32_t uiVersion;
    uint32_t uiTick;
#endif
    float fAvatarPosition[3];
    float fAvatarFront[3];
    float fAvatarTop[3];
    wchar_t name[256];
    float fCameraPosition[3];
    float fCameraFront[3];
    float fCameraTop[3];
    wchar_t identity[256];
#ifdef _WIN32
    UINT32 context_len;
#else
    uint32_t context_len;
#endif
    unsigned char context[256];
    wchar_t description[2048];
};

MumbleService::MumbleService(entt::dispatcher& aDispatcher, TransportService& aTransport)
    : m_transport(aTransport)
{
    aDispatcher.sink<UpdateEvent>().connect<&MumbleService::Update>(this);

    m_uniqueIdentifier = Utils::RandomStringW(32);

#ifdef _WIN32
    HANDLE hMapObject = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"MumbleLink");
    if (hMapObject == 0)
        return;

    m_pLinkedMem = static_cast<LinkedMem*>(MapViewOfFile(hMapObject, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkedMem)));
    if (m_pLinkedMem == nullptr)
    {
        CloseHandle(hMapObject);
        hMapObject = nullptr;
        return;
    }
#else
    char memname[256];
    snprintf(memname, 256, "/MumbleLink.%d", getuid());

    int shmfd = shm_open(memname, O_RDWR, S_IRUSR | S_IWUSR);

    if (shmfd < 0)
    {
        return;
    }

    m_pLinkedMem = static_cast<LinkedMem*>(mmap(NULL, sizeof(struct LinkedMem), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0));

    if (m_pLinkedMem == (void*)(-1))
    {
        m_pLinkedMem = nullptr;
        return;
    }
#endif
}

MumbleService::~MumbleService() = default;

void MumbleService::Update(const UpdateEvent& acEvent)
{
    float fUnitsToMeters = 70.03f;

    // TODO: Fallout4 doesn't have PlayerCamera?
#if TP_SKYRIM
    if (!m_pLinkedMem || !m_transport.IsOnline())
        return;

    auto* pPlayer = PlayerCharacter::Get();
    auto* pCamera = PlayerCamera::Get();
    if (!pPlayer || !pCamera)
        return;

    if (m_pLinkedMem->uiVersion != 2)
    {
#if TP_SKYRIM
        wcsncpy(m_pLinkedMem->name, L"SkyrimTogether", 256);
#else
        wcsncpy(m_pLinkedMem->name, L"FalloutTogether", 256);
#endif
        wcsncpy(m_pLinkedMem->description, L"Together client link.", 2048);
        m_pLinkedMem->uiVersion = 2;
    }
    m_pLinkedMem->uiTick++;

    // Left handed coordinate system.
    // X positive towards "right".
    // Y positive towards "up".
    // Z positive towards "front".
    //
    // 70.03 units = 1 meter

    auto playerRot = angleAxis(pPlayer->rotation.x, glm::vec3(1, 0, 0));
    playerRot *= angleAxis(pPlayer->rotation.y, glm::vec3(0, 1, 0));
    playerRot *= angleAxis(pPlayer->rotation.z, glm::vec3(0, 0, 1));

    const auto playerUpVector = rotate(playerRot, glm::vec3(0, 1, 0));
    const auto playerForwardVector = rotate(playerRot, glm::vec3(0, 0, -1));

    // Unit vector pointing out of the avatar's eyes aka "At"-vector.
    m_pLinkedMem->fAvatarFront[0] = playerForwardVector.x;
    m_pLinkedMem->fAvatarFront[1] = playerForwardVector.z;
    m_pLinkedMem->fAvatarFront[2] = playerForwardVector.y;

    // Unit vector pointing out of the top of the avatar's head aka "Up"-vector (here Top points straight up).
    m_pLinkedMem->fAvatarTop[0] = playerUpVector.x;
    m_pLinkedMem->fAvatarTop[1] = playerUpVector.z;
    m_pLinkedMem->fAvatarTop[2] = playerUpVector.y;

    // Position of the avatar (here standing slightly off the origin)
    m_pLinkedMem->fAvatarPosition[0] = pPlayer->position.x * fUnitsToMeters;
    m_pLinkedMem->fAvatarPosition[1] = pPlayer->position.z * fUnitsToMeters;
    m_pLinkedMem->fAvatarPosition[2] = pPlayer->position.y * fUnitsToMeters;

    spdlog::info("Player Mumble Position: x:{}, y:{}, z:{}", pPlayer->position.x * fUnitsToMeters,
                 pPlayer->position.y * fUnitsToMeters, pPlayer->position.z * fUnitsToMeters);

    // Same as avatar but for the camera.
    m_pLinkedMem->fCameraPosition[0] = pCamera->pos.x * fUnitsToMeters;
    m_pLinkedMem->fCameraPosition[1] = pCamera->pos.z * fUnitsToMeters;
    m_pLinkedMem->fCameraPosition[2] = pCamera->pos.y * fUnitsToMeters;

    auto rot = angleAxis(pCamera->pitch, glm::vec3(1, 0, 0));
    rot *= angleAxis(pCamera->yaw, glm::vec3(0, 1, 0));

    const auto upVector = rotate(rot, glm::vec3(0, 1, 0));
    const auto forwardVector = rotate(rot, glm::vec3(0, 0, -1));

    m_pLinkedMem->fCameraFront[0] = forwardVector.x;
    m_pLinkedMem->fCameraFront[1] = forwardVector.z;
    m_pLinkedMem->fCameraFront[2] = forwardVector.y;

    m_pLinkedMem->fCameraTop[0] = upVector.x;
    m_pLinkedMem->fCameraTop[1] = upVector.z;
    m_pLinkedMem->fCameraTop[2] = upVector.y;

    // Identifier which uniquely identifies a certain player in a context (e.g. the ingame name).
    wcsncpy(m_pLinkedMem->identity, m_uniqueIdentifier.c_str(), m_uniqueIdentifier.size());

    // TODO: Proper context to make sure two people connected to different servers can't head one another
    memcpy(m_pLinkedMem->context, "ContextBlob\x00\x01\x02\x03\x04", 16);
    m_pLinkedMem->context_len = 16;
#endif
}
