#include "GameServer.h"

namespace Script
{
void CreateCalendarServiceBindings(sol::state_view aState)
{
    auto calendarType =
        aState.new_usertype<CalendarService>("CalendarService", sol::meta_function::construct, sol::no_constructor);

    calendarType["get"] = []() -> CalendarService& { return GameServer::Get()->GetWorld().GetCalendarService(); };
    calendarType["GetTimeScale"] = []() { return GameServer::Get()->GetWorld().GetCalendarService().GetTimeScale(); };
}
} // namespace Script
