#include <Services/DebugService.h>
#include <Services/TransportService.h>

#include <imgui.h>

void DebugService::DrawNetworkView()
{
    if (m_transport.IsConnected())
    {
        ImGui::Begin("Network");

        auto stats = m_transport.GetStatistics();
        float protocolSent = float(stats.SentBytes) / 1024.f;
        float protocolReceived = float(stats.RecvBytes) / 1024.f;
        float uncompressedSent = float(stats.UncompressedSentBytes) / 1024.f;
        float uncompressedReceived = float(stats.UncompressedRecvBytes) / 1024.f;

        // Fill an array of contiguous float values to plot
        // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float
        // and the sizeof() of your structure in the "stride" parameter.
        static float values[90] = {};
        static int values_offset = 0;
        static double refresh_time = 0.0;
        if (refresh_time == 0.0)
            refresh_time = ImGui::GetTime();
        while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
        {
            values[values_offset] = protocolSent;
            values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
            refresh_time += 1.0f;
        }

        // Plots can display overlay texts
        // (in this example, we will display an average value)
        {
            float average = 0.0f;
            for (int n = 0; n < IM_ARRAYSIZE(values); n++)
                average += values[n];
            average /= (float)IM_ARRAYSIZE(values);
            ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, nullptr, 0.f, FLT_MAX, ImVec2(0, 160.0f));
        }


        /*
        float width = -ImGui::CalcItemWidth();
        ImGui::PushItemWidth(width);

        auto status = GetConnectionStatus();
        status.m_flOutBytesPerSec /= 1024.f;
        status.m_flInBytesPerSec /= 1024.f;

        ImGui::InputFloat("Net Out kBps", (float*)&status.m_flOutBytesPerSec, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Net In kBps", (float*)&status.m_flInBytesPerSec, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

        ImGui::InputFloat("Protocol Out kBps", (float*)&protocolSent, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("Protocol In kBps", (float*)&protocolReceived, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

        ImGui::InputFloat("User Out kBps", (float*)&uncompressedSent, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);
        ImGui::InputFloat("User In kBps", (float*)&uncompressedReceived, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

        ImGui::PopItemWidth();
        */



        ImGui::End();
    }

    // online indicator
    ImGui::GetBackgroundDrawList()->AddRectFilled(
        ImVec2(23.f, 23.f), 
        ImVec2(50.f, 50.f), m_transport.IsConnected() ? ImColor(0, 230, 64) : ImColor(240, 52, 52));
}
