//
// Created by clara on 2023-07-20.
//

#ifndef PIEZOGUI_SBCPIEZOSTRUCTS_H
#define PIEZOGUI_SBCPIEZOSTRUCTS_H

#define FILENAMELEN 36
#define TIMELEN 12
#include <vector>
#include <array>


namespace SBCPiezoGUI {

    enum ProgramState {
        Init, Acquiring, FinishAcquire, FileDisplay, Error
    };

    // controls related to each run (applied to all channels)
    struct RunControls {
        int SampleSize = -1;
        int SampleOffset = -1;
        int SampleRes = -1;
        int SampleRate = 10000000; // change to enum perhaps
        int PostTriggerSamples = 8160;
        int PreTriggerSamples = 0;
        int TriggerTimeout = -1;
        int TriggerDelay = 0;
        int Runs = 1;
        bool SaveFile = false;
        enum Formats {txt, sbc};
        Formats format = txt;
    };

    // controls related to setting triggers; you can set multiple triggers with different sources and parameters
    struct TriggerControls {
        int TriggerNum = 1;
        std::vector<int> Sources = {1};
        std::vector<int> Levels = {10};
        std::vector<int> Condition = {1};
    };

    // controls related to each channel (all channels are include in this class
    // ActiveChannels is included in cases future modification involves different numbers of active channels,
    // but this will require signficant code changes)
    struct ChannelControls {
        int ActiveChannels = 8;
        double CurrentTimeStamp = 0; // best place I can think of to store this
        int Range[8] = {2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000};
        int DCOffset[8] = {0};
        std::array<float*, 8> Buffers;
    };

    // manager for displaying already-recorded data; makes it easier to display files with multiple segments
    struct FileDisplayManager {
        std::vector<int> segments;
        std::vector<double> channels[8];
    };
}

#endif //PIEZOGUI_SBCPIEZOSTRUCTS_H
