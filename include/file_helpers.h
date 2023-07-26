//
// Created by clara on 2023-07-20.
//

#ifndef PIEZOGUI_FILE_HELPERS_H
#define PIEZOGUI_FILE_HELPERS_H

#include "SBCPiezoStructs.h"
#include <string>
#include <iostream>
#include <fstream>
#include "SBCBinaryFormat/Writers.hpp"
#include "SBCBinaryFormat/Reader.hpp"

namespace SBCPiezoGUI {

    void SaveFile(const std::string &fileName, RunControls::Formats format, ChannelControls &chans, int segID, int segSize,
             int triggerSource);
    void InitFileDisplay(const std::string& fileName, FileDisplayManager& fileDisplay, RunControls& params);
    void DisplayFile(FileDisplayManager fileDisplay, ChannelControls& chans, int& segID, int segSize);
}

#endif //PIEZOGUI_FILE_HELPERS_H
