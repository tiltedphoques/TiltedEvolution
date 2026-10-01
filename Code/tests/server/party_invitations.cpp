#include <catch2/catch.hpp>

#include <Components.h>

TEST_CASE("Party invitation acceptance enforces its deadline without a cleanup pass", "[server.party]")
{
    PartyComponent party;
    party.Invitations[1] = 60000;
    party.Invitations[2] = 60000;
    party.Invitations[3] = 60000;

    REQUIRE(party.TryConsumeInvitation(1, 59999));
    REQUIRE_FALSE(party.TryConsumeInvitation(1, 59999));
    REQUIRE_FALSE(party.TryConsumeInvitation(2, 60000));
    REQUIRE_FALSE(party.TryConsumeInvitation(3, 60001));
    REQUIRE(party.Invitations.empty());
}

TEST_CASE("Accepting an invitation cannot consume another player's invitation", "[server.party]")
{
    PartyComponent party;
    party.Invitations[10] = 60000;
    party.Invitations[20] = 61000;

    REQUIRE_FALSE(party.TryConsumeInvitation(30, 1000));
    REQUIRE(party.Invitations.size() == 2);
    REQUIRE_FALSE(party.TryConsumeInvitation(10, 60000));
    REQUIRE(party.TryConsumeInvitation(20, 60000));
    REQUIRE(party.Invitations.empty());
}

TEST_CASE("Resending an invitation renews the deadline used by cleanup and acceptance", "[server.party]")
{
    PartyComponent party;
    party.Invitations[10] = 60000;
    party.Invitations[20] = 60000;
    party.Invitations[10] = 90000;

    party.ExpireInvitations(60000);

    REQUIRE(party.Invitations.size() == 1);
    REQUIRE_FALSE(party.TryConsumeInvitation(20, 60000));
    REQUIRE(party.TryConsumeInvitation(10, 89999));
    REQUIRE(party.Invitations.empty());
}

TEST_CASE("Party invitation cleanup preserves live invitations and removes expired ones", "[server.party]")
{
    PartyComponent party;
    for (uint32_t inviter = 0; inviter < 100; ++inviter)
        party.Invitations[inviter] = 60000 + inviter;

    party.ExpireInvitations(60049);
    REQUIRE(party.Invitations.size() == 50);
    for (uint32_t inviter = 0; inviter < 100; ++inviter)
        REQUIRE(party.Invitations.contains(inviter) == (inviter >= 50));

    party.ExpireInvitations(60099);
    REQUIRE(party.Invitations.empty());
    party.ExpireInvitations(60100);
    REQUIRE(party.Invitations.empty());
}
