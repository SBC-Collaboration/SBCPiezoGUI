//
// Created by clara on 2023-06-28.
//

#ifndef PIEZOGUI_GAGE_HELPERS_H
#define PIEZOGUI_GAGE_HELPERS_H
#pragma once


// includes
#include <unordered_map>
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


namespace SBCPiezoGUI {

    int Initialize(CSHANDLE* pSystem);
    void Configure(CSHANDLE hSystem, RunControls& params, ChannelControls chans, TriggerControls& trigs);
    void AcquireChannelData(CSHANDLE hSystem, RunControls params, ChannelControls& chans, int segID, const std::string& fileName);
    void WriteINIFile(LPCTSTR fileName, RunControls params, ChannelControls chans, TriggerControls trigs); // definitely needs parameters; also will probably need to return stuff unless you read in?

    // error function
    // transfer timestamp and converttovolts



}

#endif //PIEZOGUI_GAGE_HELPERS_H
