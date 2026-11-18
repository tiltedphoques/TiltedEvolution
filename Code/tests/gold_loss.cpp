#include "../server/Services/GoldLoss.h"
#include <catch2/catch.hpp>
#include <limits>

TEST_CASE("Death gold settings distinguish fractions from flat amounts", "[gold-loss]")
{
    REQUIRE(GoldLoss::Calculate(360, 0.0f) == 0);
    REQUIRE(GoldLoss::Calculate(360, 0.1f) == 36);
    REQUIRE(GoldLoss::Calculate(360, 1.0f) == 360);
    REQUIRE(GoldLoss::Calculate(360, 10.0f) == 10);
    REQUIRE(GoldLoss::Calculate(360, 10.9f) == 10);
    REQUIRE(GoldLoss::Calculate(360, 1.01f) == 1);
    REQUIRE(GoldLoss::Calculate(3, 0.5f) == 1);
    REQUIRE(GoldLoss::Calculate(10, 0.7f) == 7);
    REQUIRE(GoldLoss::Calculate(3, 10.0f) == 3);
}

TEST_CASE("Death gold penalties never create gold or overflow", "[gold-loss]")
{
    REQUIRE(GoldLoss::Calculate(0, 10.0f) == 0);
    REQUIRE(GoldLoss::Calculate(-10, 0.5f) == 0);
    REQUIRE(GoldLoss::Calculate(360, -1.0f) == 0);
    REQUIRE(GoldLoss::Calculate(360, std::numeric_limits<float>::infinity()) == 0);
    REQUIRE(GoldLoss::Calculate(360, std::numeric_limits<float>::quiet_NaN()) == 0);
    REQUIRE(GoldLoss::Calculate(360, std::numeric_limits<float>::max()) == 360);
    constexpr int32_t kMax = std::numeric_limits<int32_t>::max();
    REQUIRE(GoldLoss::Calculate(kMax, 1.0f) == kMax);
    REQUIRE(GoldLoss::Calculate(kMax, 10.0f) == 10);
}
