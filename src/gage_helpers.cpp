//
// Created by clara on 2023-06-28.
//

#include "../include/gage_helpers.h"

namespace SBCPiezoGUI {

    ProgramState Initialize(CSHANDLE *pSystem) {
        int32 i32Status = CS_SUCCESS;
        int i;
        CSSYSTEMINFO            	CsSysInfo = {0};
        ARRAY_BOARDINFO         	*pArrayBoardInfo = nullptr;

        i32Status = CsInitialize();
        if (0 == i32Status) {
            printf("\nNo CompuScope systems found\n");
            return Error;
        }
        if (CS_FAILED(i32Status)) {
            DisplayErrorString(i32Status);
            return Error;
        }
        i32Status = CsGetSystem(pSystem, 0, 0, 0, 0);
        if (CS_FAILED(i32Status)) {
            DisplayErrorString(i32Status);
            return Error;
        }

        // this is effectively error checking for this function to make sure it gets the system correctly; you can remove this later potentially

        CsSysInfo.u32Size = sizeof(CSSYSTEMINFO);
        i32Status = CsGetSystemInfo(*pSystem, &CsSysInfo);

        pArrayBoardInfo = (ARRAY_BOARDINFO*)VirtualAlloc (nullptr, ((CsSysInfo.u32BoardCount - 1) * sizeof(CSBOARDINFO)) + sizeof(ARRAY_BOARDINFO), MEM_COMMIT, PAGE_READWRITE);
        if (!pArrayBoardInfo)
        {
            printf (_T("\nUnable to allocate memory\n"));
            return Error;
        }
        pArrayBoardInfo->u32BoardCount = CsSysInfo.u32BoardCount;
        for (i = 0; i < pArrayBoardInfo->u32BoardCount; i++)
        {
            pArrayBoardInfo->aBoardInfo[i].u32BoardIndex = i + 1;
            pArrayBoardInfo->aBoardInfo[i].u32Size = sizeof(CSBOARDINFO);
        }
        i32Status = CsGet(*pSystem, CS_BOARD_INFO, CS_ACQUISITION_CONFIGURATION, pArrayBoardInfo);

        printf(_T("\nBoard Name: %s"), CsSysInfo.strBoardName);
        for (i = 0; i < pArrayBoardInfo->u32BoardCount; i++)
        {
            printf(_T("\n\tSerial[%d]: %s"), i, pArrayBoardInfo->aBoardInfo[i].strSerialNumber);
        }
        printf(_T("\n"));
        return Init;
    }

    ProgramState Configure(CSHANDLE hSystem, RunControls& params, ChannelControls chans, TriggerControls& trigs) {

        int32 i32Status = CS_SUCCESS;
        CSSYSTEMINFO CsSysInfo = {0};
        CSACQUISITIONCONFIG     	CsAcqCfg = {0};
        CSAPPLICATIONDATA CsAppData = {0};
        uInt32                  	u32Mode;

        // write .INI file
        LPCTSTR szIniFile = ("PiezoGUI.ini");
        WriteINIFile(szIniFile, params, chans, trigs);

        // use .ini params to configure the Gage driver
        i32Status = CsAs_ConfigureSystem(hSystem, chans.ActiveChannels, trigs.TriggerNum, (LPCTSTR)szIniFile, &u32Mode);
        if (CS_FAILED(i32Status))
        {
            if (CS_INVALID_FILENAME == i32Status)
            {
                /*
                   Display message but continue on using defaults.
                   */
                printf(_T("\nCannot find %s - using default parameters."), szIniFile);
            }
            else
            {
                /*
                   Otherwise the call failed.  If the call did fail we should free the CompuScope
                   system so it's available for another application
                   */
                DisplayErrorString(i32Status);
                return Error;
            }
        }
        /*
           If the return value is greater than  1, then either the application,
           acquisition, some of the Channel and / or some of the Trigger sections
           were missing from the ini file and the default parameters were used.
           */
        if (CS_USING_DEFAULT_ACQ_DATA & i32Status)
            printf(_T("\nNo ini entry for acquisition. Using defaults."));
        if (CS_USING_DEFAULT_CHANNEL_DATA & i32Status)
            printf(_T("\nNo ini entry for one or more Channels. Using defaults for missing items."));

        if (CS_USING_DEFAULT_TRIGGER_DATA & i32Status)
            printf(_T("\nNo ini entry for one or more Triggers. Using defaults for missing items."));

        i32Status = CsAs_LoadConfiguration(hSystem, szIniFile, APPLICATION_DATA, &CsAppData);
        if (CS_FAILED(i32Status))
        {
            if (CS_INVALID_FILENAME == i32Status)
            {
                printf(_T("\nUsing default application parameters."));
            }
            else
            {
                DisplayErrorString(i32Status);
                return Error;
            }
        }
        else if (CS_USING_DEFAULT_APP_DATA & i32Status)
        {
            /*
               If the return value is CS_USING_DEFAULT_APP_DATA (defined in ConfigSystem.h)
               then there was no entry in the ini file for Application and we will use
               the application default values, which have already been set.
               */
            printf(_T("\nNo ini entry for application data. Using defaults."));
        }

        /*
           Commit the values to the driver.  This is where the values get sent to the
           hardware.  Any invalid parameters will be caught here and an error returned.
        */
        i32Status = CsDo(hSystem, ACTION_COMMIT);
        if (CS_FAILED(i32Status))
        {
            DisplayErrorString(i32Status);
            return Error;

        }
        CsAcqCfg.u32Size = sizeof(CSACQUISITIONCONFIG);
        i32Status = CsGet(hSystem, CS_ACQUISITION, CS_ACQUISITION_CONFIGURATION, &CsAcqCfg);
        if (CS_FAILED(i32Status))
        {
            DisplayErrorString(i32Status);
            return Error;
        }

        // read in defaults; we will error check this later
        params.SampleSize = (int)CsAcqCfg.u32SampleSize;
        params.SampleOffset = CsAcqCfg.i32SampleOffset;
        params.SampleRes = CsAcqCfg.i32SampleRes;
        return Acquiring;

    }

    ProgramState AcquireChannelData(CSHANDLE hSystem, RunControls params, ChannelControls& chans, int segID, const std::string& fileName) {
        void *pBuffer;
        float *pVBuffer;
        int i64Padding = 64;
        TRIGGERED_INFO_STRUCT       TriggeredInfo ={0};
        int16*                      ChannelTriggered;
        int32                   	i32Status = CS_SUCCESS;
        IN_PARAMS_TRANSFERDATA  	InData = {0};
        OUT_PARAMS_TRANSFERDATA 	OutData = {0};
        int64					i64SavedLength;
        int64					i64StartOffset = 0;


        TransferTimeStamp(hSystem, chans); // make sure this works too

        // allocate buffers for reading data from driver
        pBuffer = VirtualAlloc(nullptr, (size_t) ((params.PostTriggerSamples + params.PreTriggerSamples + i64Padding) * 8 * params.SampleSize),
                               MEM_COMMIT, PAGE_READWRITE);

        if (nullptr == pBuffer) {
            _tprintf(_T("\nUnable to allocate memory"));
            return Error;
        }


        pVBuffer = (float *) VirtualAlloc(nullptr, (size_t) ((params.PostTriggerSamples + params.PreTriggerSamples) * 8 * sizeof(float)), MEM_COMMIT,
                                          PAGE_READWRITE);
        if (nullptr == pVBuffer) {
            _tprintf(_T("\nUnable to allocate memory\n"));
            VirtualFree(pBuffer, 0, MEM_RELEASE);
            return Error;
        }

        // error checking to make sure the user did not modify driver constants
        if (params.SampleSize < 0) {
            _tprintf("Invalid SampleSize parameter; do not change this in code (should be a driver constant");
            VirtualFree(pBuffer, 0, MEM_RELEASE);
            VirtualFree(pVBuffer, 0, MEM_RELEASE);
            return Error;
        }

        /*
           Start the data acquisition
        */

        i32Status = CsDo(hSystem, ACTION_START);
        if (CS_FAILED(i32Status)) {
            DisplayErrorString(i32Status);
            return Error;
        }

        if (!DataCaptureComplete(hSystem)) {
            return Error;
        }

        // get what channel triggered data recording
        ChannelTriggered = (int16 *) VirtualAlloc(nullptr, (size_t) (sizeof(int16)), MEM_COMMIT, PAGE_READWRITE);
        ZeroMemory(ChannelTriggered, sizeof(int16));

        TriggeredInfo.u32Size = sizeof(TriggeredInfo);
        TriggeredInfo.u32StartSegment = 1;                      // the first segment
        TriggeredInfo.u32NumOfSegments = 1;
        TriggeredInfo.u32BufferSize = sizeof(int16);
        TriggeredInfo.pBuffer = ChannelTriggered;
        i32Status = CsGet(hSystem, 0, CS_TRIGGERED_INFO, &TriggeredInfo);
        if (CS_FAILED(i32Status)) {
            if (CS_INVALID_PARAMS_ID != i32Status)
                DisplayErrorString(i32Status);
            return Error;
        }


        /*
           Acquisition is now complete.


           Fill in the InData structure for transferring the data
           */
        /*
           Non multiple record captures should have the segment set to 1.
           InData.u32Mode refers to the transfer mode. Regular transfer is 0
           */
        InData.u32Segment = 1;
        InData.u32Mode = TxMODE_DATA_INTERLEAVED; // i do this because something is wrong with the default transfer mode
        InData.u16Channel = 1;
        /*
        Validate the start address and the length.  This is especially necessary if
        trigger delay is being used
        */

        InData.i64StartAddress = -(params.PreTriggerSamples);
        /*
            We transfer a little more than we need so we're sure to get what we requested, regardless of any hw alignment issuses
        */
        InData.i64Length = (params.PostTriggerSamples + params.PreTriggerSamples + i64Padding) * 8;
        InData.pDataBuffer = pBuffer;


        ZeroMemory(pBuffer, (size_t) ((params.PostTriggerSamples + params.PreTriggerSamples + i64Padding) * 8 *
                                      params.SampleSize));
        /*
           Transfer the captured data
        */
        i32Status = CsTransfer(hSystem, &InData, &OutData);
        if (CS_FAILED(i32Status)) {
            DisplayErrorString(i32Status);
            if (nullptr != pBuffer)
                VirtualFree(pBuffer, 0, MEM_RELEASE);
            if (nullptr != pVBuffer)
                VirtualFree(pVBuffer, 0, MEM_RELEASE);
            return Error;
        }

        /*
         * Error checking on start offset/data transfer
         */

        i64StartOffset = InData.i64StartAddress - OutData.i64ActualStart;
        if (i64StartOffset < 0) {
            i64StartOffset = 0;
            InData.i64StartAddress = OutData.i64ActualStart;
        }


        /*
        Save the smaller of the requested transfer length or the actual transferred length
        */
        OutData.i64ActualLength += i64StartOffset;
        i64SavedLength = (params.PostTriggerSamples + params.PreTriggerSamples) * 8 <= OutData.i64ActualLength ?
                         (params.PostTriggerSamples + params.PreTriggerSamples) * 8
                                                                                                                   : OutData.i64ActualLength;

/*
				Call the ConvertToVolts function. This function will convert the raw
				data to voltages. We pass the buffer plus the actual start, which will be converted
				from reqested start to actual length in the volts buffer.
*/
        // Check if this works lol
        i32Status = ConvertToVoltsInterleaved((int)i64SavedLength, params, chans, pBuffer);

        if (CS_FAILED(i32Status)) {
            DisplayErrorString(i32Status);
            return Error;
        }

        if (params.SaveFile) {
            SaveFile(fileName, params.format, chans, segID, params.PreTriggerSamples + params.PostTriggerSamples, ChannelTriggered[0]);
        }

        VirtualFree(pBuffer, 0, MEM_RELEASE);
        VirtualFree(pVBuffer, 0, MEM_RELEASE);
        return Acquiring;

    }

    // Write .ini files in the proper format based on user input
    void WriteINIFile(LPCTSTR fileName, RunControls params, ChannelControls chans, TriggerControls trigs) {
        std::ofstream iniFile;
        iniFile.open(fileName);
        iniFile << "[Acquisition]\nMode=Octal\nSampleRate=" << params.SampleRate << std::endl;
        // remember to change this to actually get desired depth inc. pre-trig
        iniFile << "Depth=" << params.PostTriggerSamples << "\nSegmentSize=" << (params.PostTriggerSamples + params.PreTriggerSamples) << std::endl;
        iniFile << "SegmentCount=1\nTriggerHoldoff=" << params.PreTriggerSamples << "\nTriggerTimeout=" << params.TriggerTimeout << std::endl;
        iniFile << "TriggerDelay=" << params.TriggerDelay << "\nTimeStampMode=Free\nTimeStampClock=Fixed\n" << std::endl;
        for (int i = 0; i < chans.ActiveChannels; i++) {
            iniFile << "[Channel" << (i+1) << "]\nRange=" << chans.Range[i] << "\nCoupling=DC\nImpedance=50\nDcOffset=" << chans.DCOffset[i] << "\n" << std::endl;
        }
        for (int i = 0; i < trigs.TriggerNum; i++) {
            iniFile << "[Trigger" << (i+1) << "]" << std::endl;
            if (trigs.Condition[i] == 0)  // falling
                iniFile << "Condition=Falling" << std::endl;
            else
                iniFile << "Condition=Rising" << std::endl;
            iniFile << "Level=" << trigs.Levels[i] << std::endl;
            if (trigs.Sources[i] == 0) {
                // change these as necessary depending on what ext trigger you're using
                iniFile << "Source=External\nCoupling=DC\nImpedance=50\n" << std::endl;
            }
            else
                iniFile << "Source=" << trigs.Sources[i] << "\n" << std::endl;
        }
        // again here you need to make sure your total samples are ok
        iniFile << "[Application]\nStartPosition=" << -(params.PreTriggerSamples) << "\nTransferLength=" << params.PostTriggerSamples + params.PreTriggerSamples << std::endl;
        iniFile << "SegmentStart=1\nSegmentCount=1\nSaveFileFormat=TYPE_FLOAT" << std::endl;
        iniFile.close();
    }

    int32 ConvertToVoltsInterleaved(int64 depth, RunControls params, ChannelControls& chans, void* pBuffer) {
/*      Converts the raw data in the buffer pBuffer to voltages and puts them into pVBuffer.
        The conversion is done using the formula:
        voltage = ((sample_offset - raw_value) / sample_resolution) * gain
        where gain is calculated in volts by gain_in_millivolts / CS_GAIN_2_V (2000).
        The DC Offset is added to each element of the voltage buffer
*/

        double                                  dScaleFactor[8];
        int64                                   i;
        int32                                   i32Status = CS_SUCCESS;

        for (int j = 0; j < 8; j++) {
            dScaleFactor[j] = (double)(chans.Range[j]) / (double)(CS_GAIN_2_V);
            chans.Buffers[j] = new float[params.PostTriggerSamples + params.PreTriggerSamples];

        }
        int counter = 0;
        switch (params.SampleSize)
        {
            double dOffset;
            double dValue;
            uInt8 *p8ShortBuffer;
            int16 *p16ShortBuffer;
            int32 *p32ShortBuffer;

            case 1:
                p8ShortBuffer = (uInt8 *)pBuffer;
                for (i = 0; i < depth; i+=8)
                {
                    for (int j = 0; j < 8; j++) {
                        dOffset = params.SampleOffset - (double)(p8ShortBuffer[i+j]);
                        dValue = dOffset / (double)params.SampleRes;
                        dValue *= dScaleFactor[j];
                        dValue += (double)(chans.DCOffset[j]) / 1000.0;
                        chans.Buffers[j][counter] = (float)dValue;
                    }
                    counter++;
                }
                break;
            case 2:
                p16ShortBuffer = (int16 *)pBuffer;
                for (i = 0; i < depth; i+=8)
                {
                    for (int j = 0; j < 8; j++) {
                        dOffset = params.SampleOffset - (double)(p16ShortBuffer[i+j]);
                        dValue = dOffset / (double)params.SampleRes;
                        dValue *= dScaleFactor[j];
                        dValue += (double)(chans.DCOffset[j]) / 1000.0;
                        chans.Buffers[j][counter] = (float)dValue;
                    }
                    counter++;
                }
                break;

            case 4:
                p32ShortBuffer = (int32 *)pBuffer;
                for (i = 0; i < depth; i+=8)
                {
                    for (int j = 0; j < 8; j++) {
                        dOffset = params.SampleOffset - (double)(p32ShortBuffer[i+j]);
                        dValue = dOffset / (double)params.SampleRes;
                        dValue *= dScaleFactor[j];
                        dValue += (double)(chans.DCOffset[j]) / 1000.0;
                        chans.Buffers[j][counter] = (float)dValue;
                    }
                    counter++;
                }
                break;

            default:
                i32Status = CS_MISC_ERROR;
                break;
        }

        return i32Status;
    }

    void TransferTimeStamp(CSHANDLE hSystem, ChannelControls& chans)
    {
        IN_PARAMS_TRANSFERDATA              InTSData = {0};
        OUT_PARAMS_TRANSFERDATA             OutTSData = {0};
        int32                                               i32Status = CS_SUCCESS;
        int64                                               i64TickFrequency = 0;

        InTSData.u16Channel = 1;
        InTSData.u32Mode = TxMODE_TIMESTAMP;
        InTSData.i64StartAddress = 1;
        InTSData.i64Length = 1;
        InTSData.u32Segment = 1;

        int64* pTimeStamp;
        pTimeStamp = (int64 *)VirtualAlloc( nullptr, (size_t)(sizeof(int64)), MEM_COMMIT, PAGE_READWRITE);
        if (nullptr == pTimeStamp)
        {
            _tprintf (_T("\nUnable to allocate memory\n"));
        }
        ZeroMemory(pTimeStamp,(size_t)(sizeof(int64)));
        InTSData.pDataBuffer = pTimeStamp;

        i32Status = CsTransfer(hSystem, &InTSData, &OutTSData);
        if (CS_FAILED(i32Status))
        {
/*
                if the error is INVALID_TRANSFER_MODE it just means that this systems
                doesn't support time stamp. We can continue on with the program.
*/
            if (CS_INVALID_TRANSFER_MODE == i32Status)
                _tprintf (_T("\nTime stamp is not supported in this system.\n"));
            else
                DisplayErrorString(i32Status);

            VirtualFree(pTimeStamp, 0, MEM_RELEASE);
            pTimeStamp = nullptr;
        }

        i32Status = CsGet(hSystem, CS_PARAMS, CS_TIMESTAMP_TICKFREQUENCY, &i64TickFrequency);
        if (CS_FAILED(i32Status))
        {
            DisplayErrorString(i32Status);
        }


        if (CS_SUCCEEDED(i64TickFrequency))
        {
            /*
                The number of ticks that have ocurred / tick count(the number of ticks / second)
                = the number of seconds elapsed. Multiple by 1000000 to get the number of
                mircoseconds
            */
            chans.CurrentTimeStamp = (double)( (*pTimeStamp)) / (double)(i64TickFrequency);
        }

    }

}
