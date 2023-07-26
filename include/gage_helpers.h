//
// Created by clara on 2023-06-28.
//

#ifndef PIEZOGUI_GAGE_HELPERS_H
#define PIEZOGUI_GAGE_HELPERS_H
#pragma once

#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include "file_helpers.h"
#include "CsPrototypes.h"
#include "CsAppSupport.h"
#include "CsTchar.h"
#include "CsSdkMisc.h"
#undef max

namespace SBCPiezoGUI {

    ProgramState Initialize(CSHANDLE* pSystem);
    ProgramState Configure(CSHANDLE hSystem, RunControls& params, ChannelControls chans, TriggerControls& trigs);
    ProgramState AcquireChannelData(CSHANDLE hSystem, RunControls params, ChannelControls& chans, int segID, const std::string& fileName);
    void WriteINIFile(LPCTSTR fileName, RunControls params, ChannelControls chans, TriggerControls trigs); // definitely needs parameters; also will probably need to return stuff unless you read in?
    void TransferTimeStamp(CSHANDLE hSystem, ChannelControls& chans);
    int32 ConvertToVoltsInterleaved(int64 depth, RunControls params, ChannelControls& chans, void* pBuffer);
}

#endif //PIEZOGUI_GAGE_HELPERS_H
