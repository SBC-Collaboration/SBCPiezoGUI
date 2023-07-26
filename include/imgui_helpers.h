//
// Created by clara on 2023-06-28.
//

#ifndef PIEZOGUI_IMGUI_HELPERS_H
#define PIEZOGUI_IMGUI_HELPERS_H
#pragma once
#include <iostream>
#include <string>
#include <array>
#ifdef WIN32
#include <format>
#else
#define FMT_HEADER_ONLY
#include <fmt/core.h>
#endif
#include <unordered_map>
#include <stdio.h>
#include <string.h>
#include "gage_helpers.h"
#include "imgui.h"
#include "implot.h"
#include "file_helpers.h"
#include "ImGuiFileDialog.h"
#undef max


namespace SBCPiezoGUI {

    class WindowManager {
        ProgramState state;
        RunControls params;
        ChannelControls channels;
        TriggerControls triggers;
        FileDisplayManager displayManager;

        // Input option arrays
        // check what the allowed options on the driver are
        const char* sampleRates[17] = {"100 MS/s", "50 MS/s", "25 MS/s", "12.5 MS/s", "10 MS/s", "5 MS/s", "2 MS/s", "1 MS/s", "500 kS/s", "200 kS/s", "100 kS/s", "50 kS/s", "20 kS/s", "10 kS/s", "5 kS/s", "2 kS/s", "1 kS/s"};
        int sampleRatesInt[17] = {100000000, 50000000, 25000000, 12500000, 10000000, 5000000, 2000000, 1000000, 500000, 200000, 100000, 50000, 20000, 10000, 5000, 2000, 1000};
        // unordered_map here or in the .ini file function?
        const char* channelNumbers[8] = {"Channel 1", "Channel 2"
                                         , "Channel 3", "Channel 4", "Channel 5", "Channel 6", "Channel 7", "Channel 8"};
        const char* inputRanges[2] = {"2000", "10000"};
        int inputRangesInt[2] = {2000, 10000}; // this is so stupid but there's no other way for me to do this lol
        const char* triggerConditions[2] = {"Falling", "Rising"};
        const char* formatOptions[2] = {".txt", ".sbc.bin"};
        const char* triggerSources[9] = {"External", "Channel 1", "Channel 2", "Channel 3", "Channel 4", "Channel 5", "Channel 6", "Channel 7", "Channel 8"};

        // Input storage variables
        int currentChannel;
        int currentSampleRate;
        int currentInputRange;
        int currentFormat;
        int currentSegment;

        char fileName[FILENAMELEN];

    public:
        WindowManager() {
            state = Init;
            params = {};
            channels = {};
            triggers = {};

            currentChannel = 0;
            currentSampleRate = 0;
            currentInputRange = 0;
            currentFormat = 0;
            currentSegment = 0;

        }
        void DrawTabBar(CSHANDLE hSystem);
        void DrawAcqTab();
        void DrawChannelTab();
        void DrawTriggerTab();
        void DrawDisplayTab();
        void DrawChannels();

        ProgramState GetState() { return state; }
        void SetState(ProgramState newState) { state = newState; }
    };

    bool BackEndInitialize();

    void WriteFileName(RunControls::Formats format, char (&fileName)[FILENAMELEN]);
}
#endif //PIEZOGUI_IMGUI_HELPERS_H
