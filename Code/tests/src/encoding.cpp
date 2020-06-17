#include <fstream>
#include <ActionEvent.h>
#include <iostream>
#include <Stl.hpp>
#include <iomanip>

void RunEncodingTests()
{
    TiltedPhoques::Map<uint32_t, TiltedPhoques::Vector<ActionEvent>> events;

    std::ifstream animDump{ "animation_dump.txt", std::ios::binary};
    while(!animDump.eof() && animDump.is_open())
    {
        ActionEvent evt;
        evt.Load(animDump);

        if (evt.ActorId == 0)
            break;

        //if(evt.ActionId != 0x13002)
        events[evt.ActorId].push_back(evt);
    }

    uint64_t totalData = 0;
    uint64_t eventCount = 0;

    for(auto& vec : events)
    {
        auto& eventVector = vec.second;

        eventCount += eventVector.size();

        if(eventVector.size() > 1)
        {
            TiltedPhoques::Buffer buff(8000);

            uint64_t totalSessionData = 0;

            auto current = eventVector[0];
            for(auto i = 1u; i < eventVector.size(); ++i)
            {
                auto next = eventVector[i];

                TiltedPhoques::Buffer::Writer writer(&buff);
                TiltedPhoques::Buffer::Reader reader(&buff);

                next.GenerateDiff(current, writer);

                totalSessionData += writer.GetBytePosition();

                current.ApplyDiff(reader);

                if(current != next)
                {
                    std::cout << "FAILED" << std::endl;
                }
            }

            totalData += totalSessionData;

            std::cout << std::hex << "Id: " << vec.first << " has " << std::dec << vec.second.size() << " events, cost average: "
                << std::setprecision(3) << float(totalSessionData) / eventVector.size() << " bytes/event" << std::endl;
        }
    }

    std::cout << std::dec << eventCount << " events, total cost : " << totalData << " cost average: "
        << std::setprecision(3) << float(totalData) / eventCount << " bytes/event" << std::endl;
}
