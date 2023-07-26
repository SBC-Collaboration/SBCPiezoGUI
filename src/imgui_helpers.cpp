//
// Created by clara on 2023-06-28.
//

#include "../include/imgui_helpers.h"

namespace SBCPiezoGUI {

    void WindowManager::DrawTabBar(CSHANDLE hSystem) {
        ImGui::SetNextWindowSize(ImVec2(500,300), ImGuiCond_Appearing);
        ImGui::Begin("Items");
        if (ImGui::BeginTabBar("Options", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Acquisition Options"))
            {
                DrawAcqTab();
                if (state == Acquiring) {
                    if (currentSegment == 0) {
                        state = Configure(hSystem, params, channels, triggers);
                        WriteFileName(params.format, fileName);
                    }
                    if (state != Error) {
                        state = AcquireChannelData(hSystem, params, channels, currentSegment, fileName);
                        currentSegment++;
                        if (currentSegment >= params.Runs && state != Error) {
                            state = FinishAcquire;
                            currentSegment = 0;
                        }
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
        ImGui::Combo("Sample Rate", &currentSampleRate, sampleRates, 3);
        params.SampleRate = sampleRatesInt[currentSampleRate];
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
        if (channels.Range[currentChannel] == 2000)
            currentInputRange = 0;
        else
            currentInputRange = 1;
        ImGui::Combo("Input Range [mV]", &currentInputRange, inputRanges, 2);
        channels.Range[currentChannel] = inputRangesInt[currentInputRange];
        ImGui::InputInt("DC Offset [%]", &channels.DCOffset[currentChannel]);
    }


    void WindowManager::DrawTriggerTab() {
        bool edited = false;
        if (ImGui::InputInt("Number of Trigger Sources", &triggers.TriggerNum)) {
            edited = true;
        }
        if (edited) {
            triggers.Sources.push_back(1);
            triggers.Levels.push_back(10);
            triggers.Condition.push_back(1);
        }

        for (int i = 0; i < triggers.TriggerNum; i++) {
#ifdef WIN32
            ImGui::Combo(std::format("Source {}", i+1).data(), &triggers.Sources[i], triggerSources, 9);
            ImGui::InputInt(std::format("Level {} [%]", i+1).data(), &triggers.Levels[i]);
            ImGui::Combo(std::format("Condition {}", i+1).data(), &triggers.Condition[i], triggerConditions, 2);
#else
            ImGui::Combo(fmt::format("Source {}", i+1).data(), &triggers.Sources[i], triggerSources, 9);
            ImGui::InputInt(fmt::format("Level {} [%]", i+1).data(), &triggers.Levels[i]);
            ImGui::Combo(fmt::format("Condition {}", i+1).data(), &triggers.Condition[i], triggerConditions, 2);
#endif
        }
    }

    void WindowManager::DrawDisplayTab() {
        if (ImGui::Button("Open File")) {
            ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Appearing);
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".bin", ".");
        }
        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                InitFileDisplay(filePathName, displayManager, params);
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
            DisplayFile(displayManager, channels, currentSegment, params.PostTriggerSamples+params.PreTriggerSamples);
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
                    if (state == Init || state == Error) {
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
