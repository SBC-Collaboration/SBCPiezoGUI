//
// Created by clara on 2023-06-28.
//

#include "../include/imgui_helpers.h"

namespace SBCPiezoGUI {

    void WindowManager::DrawTabBar(CSHANDLE hSystem) {
        ImGui::SetNextWindowSize(ImVec2(415,350), ImGuiCond_Appearing);
        ImGui::Begin("Items");
        if (ImGui::BeginTabBar("Options", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Acquisition Options"))
            {
                DrawAcqTab();
                // maybe move this somewhere else? like have a functionality thing? if you're already separating by displays...idk
                if (state == Acquiring) {
                    if (currentSegment == 0) {
                        Configure(hSystem, params, channels, triggers);
                        WriteFileName(params.format, fileName);
                    }
                    AcquireChannelData(hSystem, params, channels, currentSegment, fileName);
                    currentSegment++;
                    if (currentSegment >= params.Runs) {
                        state = FinishAcquire;
                        currentSegment = 0;
                    }
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Channel Options"))
            {
                DrawChannelTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Trigger Options"))
            {
                DrawTriggerTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Display Options"))
            {
                DrawDisplayTab();
                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
        ImGui::End();
    }

    void WindowManager::DrawAcqTab() {
        ImGui::InputInt("Sample Rate", &params.SampleRate);
        ImGui::InputInt("Post Trigger Samples", &params.PostTriggerSamples);
        ImGui::InputInt("Pre Trigger Samples", &params.PreTriggerSamples);
        ImGui::InputInt("Trigger Timeout", &params.TriggerTimeout);
        ImGui::InputInt("Trigger Delay", &params.TriggerDelay);
        ImGui::InputInt("Runs", &params.Runs);
        ImGui::Checkbox("Save File?", &params.SaveFile);
        if (params.SaveFile) {
            ImGui::Combo("File Format", &currentFormat, formatOptions, 2);
            if (currentFormat == 0)
                params.format = RunControls::Formats::txt;
            else
                params.format = RunControls::Formats::sbc;
            if (ImGui::Button("Save Current Signals")) {
                char fileName[FILENAMELEN];
                WriteFileName(params.format, fileName);
                SaveFile(fileName, params.format, channels, 0, params.PreTriggerSamples + params.PostTriggerSamples, 9);
            }
        }
        if (ImGui::Button("Acquire")) {
            state = Acquiring;
            currentSegment = 0;
        }
    }

    void WindowManager::DrawChannelTab() {
        ImGui::Combo("Channels", &currentChannel, channelNumbers, 8);
        ImGui::InputInt("Input Range", &channels.Range[currentChannel]); // this doesn't work? do some print tests/check ini file to confirm cause it may be a vis issue
        ImGui::InputInt("DC Offset", &channels.DCOffset[currentChannel]);
    }

    void WindowManager::DrawTriggerTab() {
        ImGui::InputInt("Number of Trigger Sources", &triggers.TriggerNum);
        for (int i = 0; i < triggers.TriggerNum; i++) {
            triggers.Sources.push_back(1);
            triggers.Levels.push_back(10);
            triggers.Condition.push_back(1);
            ImGui::Combo("Source", &triggers.Sources[i], triggerSources, 9);
            ImGui::InputInt("Level", &triggers.Levels[i]);
            ImGui::Combo("Condition", &triggers.Condition[i], triggerConditions, 2);
        }
    }

    void WindowManager::DrawDisplayTab() {
        // window size needs to be adjusted but otherwise this works
        if (ImGui::Button("Open File"))
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".bin", ".");

        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                channels = ChannelControls(params.PostTriggerSamples + params.PreTriggerSamples);
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                InitFileDisplay(filePathName, displayManager);
                state = FileDisplay;
                // action
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
        if (state == FileDisplay) {
            if (ImGui::Button("->")) {
                currentSegment++;
            }
            if (ImGui::Button("<-")) {
                currentSegment--;
            }
            ImGui::InputInt("Current Segment", &currentSegment);
            DisplayFile(displayManager, channels, currentSegment, params.PreTriggerSamples + params.PostTriggerSamples);
        }
    }

    void WindowManager::DrawChannels() {
        ImGui::SetNextWindowSize(ImVec2(400*4, 280*2), ImGuiCond_Appearing);

        ImGui::Begin("Channels");
        if (ImPlot::BeginSubplots("Channels", 2, 4, ImVec2(400*4, 280*2)))
            for (int i = 0; i < 8; i++) {
#ifdef WIN32
                std::string PlotTitle = std::format("Channel {}", i+1);
#else
                std::string PlotTitle = fmt::format("Channel {}", i+1);
#endif
                if (ImPlot::BeginPlot(PlotTitle.data())) {
                    ImPlot::SetupAxes("Sample Number","Signal [V]");
                    ImPlot::SetupAxesLimits(0, params.PreTriggerSamples + params.PostTriggerSamples, -(channels.Range[i]/2. /1000), channels.Range[i]/2. /1000);
                    if (state == Init) {
                        ImPlot::PlotDummy(PlotTitle.data()); // come back to this for channel formatting
                    }
                    else {
                        ImPlot::PlotLine(PlotTitle.data(), channels.Buffers[i], params.PreTriggerSamples + params.PostTriggerSamples);
                    }
                    ImPlot::EndPlot();
                }
            }
        ImPlot::EndSubplots();
        ImGui::End();
    }

    void WriteFileName(RunControls::Formats format, char (& fileName)[FILENAMELEN]) {
        char timeStr[TIMELEN];
        time_t rawtime;
        struct tm *info;
        time(&rawtime);
        info = localtime(&rawtime);
        strftime(timeStr, sizeof(timeStr), "%H%M%S-%m%d", info);

        if (format == RunControls::txt)
            sprintf(fileName, "acoustic-%s.txt", timeStr);
        else
            sprintf(fileName, "acoustic-%s.sbc.bin", timeStr);
    }

}
