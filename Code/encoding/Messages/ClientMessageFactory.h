#pragma once

#include <Messages/Message.h>
#include <MetaMessage.h>

#include <Messages/AuthenticationRequest.h>
#include <Messages/AssignCharacterRequest.h>
#include <Messages/CancelAssignmentRequest.h>
#include <Messages/ClientReferencesMoveRequest.h>
#include <Messages/EnterInteriorCellRequest.h>
#include <Messages/ClientRpcCalls.h>
#include <Messages/RequestInventoryChanges.h>
#include <Messages/RequestFactionsChanges.h>
#include <Messages/RequestQuestUpdate.h>
#include <Messages/PartyInviteRequest.h>
#include <Messages/PartyAcceptInviteRequest.h>
#include <Messages/PartyLeaveRequest.h>
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

using TiltedPhoques::UniquePtr;

struct ClientMessageFactory
{
    UniquePtr<ClientMessage> Extract(TiltedPhoques::Buffer::Reader& aReader) const noexcept;

    template <class T>
    static auto Visit(T&& func)
    {
        auto s_visitor = CreateMessageVisitor<AuthenticationRequest, AssignCharacterRequest, CancelAssignmentRequest,
                                 ClientReferencesMoveRequest, EnterInteriorCellRequest, ClientRpcCalls,
                                 RequestInventoryChanges, RequestFactionsChanges, RequestQuestUpdate,
                                 PartyInviteRequest, PartyAcceptInviteRequest, PartyLeaveRequest,
                                 RequestActorValueChanges, RequestActorMaxValueChanges, EnterExteriorCellRequest,
                                 RequestHealthChangeBroadcast, RequestSpawnData, ActivateRequest, LockChangeRequest,
                                 AssignObjectsRequest, RequestDeathStateChange, ShiftGridCellRequest, RequestOwnershipTransfer,
                                 RequestOwnershipClaim>;

        return s_visitor(std::forward<T>(func));
    }
};
