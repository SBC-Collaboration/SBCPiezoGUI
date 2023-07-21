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
    enum ProgramState {
        Init, Acquiring, FinishAcquire, FileDisplay
    }; // these can be maybe named more descriptively idk
    class WindowManager {
        ProgramState state;
        RunControls params;
        ChannelControls channels;
        TriggerControls triggers;
        FileDisplayManager displayManager; // we'll see if this needs to be a class variable

        // Input option arrays
        // check what the allowed options on the driver are
        const char* sampleRates[3] = {"100000000", "10000000", "1000000"};
        // unordered_map here or in the .ini file function?
        const char* channelNumbers[8] = {"Channel 1", "Channel 2", "Channel 3", "Channel 4", "Channel 5", "Channel 6", "Channel 7", "Channel 8"};
        const char* inputRanges[2] = {"2000", "10000"};
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
            params = {}; // make sure this works
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
        void DrawChannels(); // just move plotchannels here
        ProgramState GetState() { return state; }
        // writefilename? we'll see
    };

    // change windows to classes; do config/draw methods

    bool BackEndInitialize();

    void WriteFileName(RunControls::Formats format, char (&fileName)[FILENAMELEN]);
}
#endif //PIEZOGUI_IMGUI_HELPERS_H
