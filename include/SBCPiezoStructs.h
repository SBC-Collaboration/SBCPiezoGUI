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

    struct RunControls {
        // at least the negative on sample size can be used for error checking
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

    struct TriggerControls {
        int TriggerNum = 1;
        std::vector<int> Sources = {1};
        std::vector<int> Levels = {10};
        std::vector<int> Condition = {1};
    };

    struct ChannelControls {
        int ActiveChannels = 8;
        int Range[8] = {2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000};
        int DCOffset[8] = {0};
        std::array<float*, 8> Buffers;
    };

    struct FileDisplayManager {
        std::vector<int> segments;
        std::vector<float> channels[8];
    };
}

#endif //PIEZOGUI_SBCPIEZOSTRUCTS_H
