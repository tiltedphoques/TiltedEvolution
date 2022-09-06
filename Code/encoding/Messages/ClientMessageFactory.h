#pragma once

#include <Messages/Message.h>
#include <MetaMessage.h>

#include <Messages/AuthenticationRequest.h>
#include <Messages/AssignCharacterRequest.h>
#include <Messages/CancelAssignmentRequest.h>
#include <Messages/ClientReferencesMoveRequest.h>
#include <Messages/EnterInteriorCellRequest.h>
#include <Messages/RequestInventoryChanges.h>
#include <Messages/RequestFactionsChanges.h>
#include <Messages/RequestQuestUpdate.h>
#include <Messages/PartyInviteRequest.h>
#include <Messages/PartyAcceptInviteRequest.h>
#include <Messages/PartyLeaveRequest.h>
#include <Messages/PartyCreateRequest.h>
#include <Messages/PartyChangeLeaderRequest.h>
#include <Messages/PartyKickRequest.h>
#include <Messages/PartyInviteRequest.h>
#include <Messages/RequestActorValueChanges.h>
#include <Messages/RequestActorMaxValueChanges.h>
#include <Messages/RequestHealthChangeBroadcast.h>
#include <Messages/RequestSpawnData.h>
#include <Messages/ActivateRequest.h>
#include <Messages/LockChangeRequest.h>
#include <Messages/AssignObjectsRequest.h>
#include <Messages/RequestDeathStateChange.h>
#include <Messages/ShiftGridCellRequest.h>
#include <Messages/RequestOwnershipTransfer.h>
#include <Messages/EnterExteriorCellRequest.h>
#include <Messages/RequestOwnershipClaim.h>
#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/SpellCastRequest.h>
#include <Messages/InterruptCastRequest.h>
#include <Messages/AddTargetRequest.h>
#include <Messages/ProjectileLaunchRequest.h>
#include <Messages/ScriptAnimationRequest.h>
#include <Messages/DrawWeaponRequest.h>
#include <Messages/MountRequest.h>
#include <Messages/NewPackageRequest.h>
#include <Messages/RequestRespawn.h>
#include <Messages/SyncExperienceRequest.h>
#include <Messages/RequestEquipmentChanges.h>
#include <Messages/SendChatMessageRequest.h>
#include <Messages/TeleportCommandRequest.h>
#include <Messages/PlayerRespawnRequest.h>
#include <Messages/DialogueRequest.h>
#include <Messages/SubtitleRequest.h>
#include <Messages/PlayerDialogueRequest.h>
#include <Messages/PlayerLevelRequest.h>
#include <Messages/TeleportRequest.h>
#include <Messages/RequestPlayerHealthUpdate.h>
#include <Messages/RequestWeatherChange.h>
#include <Messages/RequestCurrentWeather.h>

using TiltedPhoques::UniquePtr;

struct ClientMessageFactory
{
    UniquePtr<ClientMessage> Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept;

    template <class T>
    static auto Visit(T&& func)
    {
        auto s_visitor = CreateMessageVisitor<AuthenticationRequest, AssignCharacterRequest, CancelAssignmentRequest,
                                 ClientReferencesMoveRequest, EnterInteriorCellRequest,
                                 RequestInventoryChanges, RequestFactionsChanges, RequestQuestUpdate,
                                 PartyInviteRequest, PartyAcceptInviteRequest, PartyLeaveRequest, PartyCreateRequest, PartyChangeLeaderRequest, PartyKickRequest,
                                 RequestActorValueChanges, RequestActorMaxValueChanges, EnterExteriorCellRequest,
                                 RequestHealthChangeBroadcast, RequestSpawnData, ActivateRequest, LockChangeRequest,
                                 AssignObjectsRequest, RequestDeathStateChange, ShiftGridCellRequest, RequestOwnershipTransfer,
                                 RequestOwnershipClaim, RequestObjectInventoryChanges, SpellCastRequest, ProjectileLaunchRequest, InterruptCastRequest,
                                 AddTargetRequest, ScriptAnimationRequest, DrawWeaponRequest, MountRequest, NewPackageRequest,
                                 RequestRespawn, SyncExperienceRequest, RequestEquipmentChanges, SendChatMessageRequest,
                                 TeleportCommandRequest, PlayerRespawnRequest, DialogueRequest, SubtitleRequest, PlayerDialogueRequest,
                                 PlayerLevelRequest, TeleportRequest, RequestPlayerHealthUpdate, RequestWeatherChange, RequestCurrentWeather>;

        return s_visitor(std::forward<T>(func));
    }
};
