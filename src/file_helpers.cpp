//
// Created by clara on 2023-07-20.
//

#include "../include/file_helpers.h"
namespace SBCPiezoGUI {

    void SaveFile(const std::string& fileName, RunControls::Formats format, ChannelControls& chans, int segID, int segSize, int triggerSource) {
        int chanTrig;
        std::ofstream dataFile;

        switch (format) {
            case RunControls::txt:
                dataFile.open(fileName, std::ios::app);
                if (segID == 0) {
                    dataFile
                            << "SegmentID,TriggerSource,Channel1,Channel2,Channel3,Channel4,Channel5,Channel6.Channel7,Channel8"
                            << std::endl;
                    dataFile << "-1,-1," << chans.Range[0] << "," << chans.Range[1] << "," << chans.Range[2] << ","
                             << chans.Range[3] << "," << chans.Range[4] << "," << chans.Range[5] << ","
                             << chans.Range[6] << "," << chans.Range[7] << std::endl;
                    dataFile << "-1,-1," << chans.DCOffset[0] << "," << chans.DCOffset[1] << ","
                             << chans.DCOffset[2] << "," << chans.DCOffset[3] << "," << chans.DCOffset[4] << ","
                             << chans.DCOffset[5] << "," << chans.DCOffset[6] << "," << chans.DCOffset[7]
                             << std::endl;
                }
                for (int i = 0; i < segSize; i++) {
                    if (i != 0) {
                        if (triggerSource == 0) // force trigger = 9
                            chanTrig = 9;
                        else if (triggerSource == -1) // ext trigger = 0
                            chanTrig = 0;
                        else
                            chanTrig = triggerSource; // channel trigger
                        dataFile << segID << "," << chanTrig << "," << chans.Buffers[0][i] << "," << chans.Buffers[1][i] << ","
                                 << chans.Buffers[2][i] << "," << chans.Buffers[3][i] << "," << chans.Buffers[4][i] << "," <<
                                 chans.Buffers[5][i] << "," << chans.Buffers[6][i] << "," << chans.Buffers[7][i] << std::endl;
                    }
                }

                dataFile.close();
                break;

            case RunControls::sbc:
                using TestWriter = SBC::BinaryFormat::DynamicWriter<int, int, float, float, float, float, float, float, float, float>;
                std::array<std::string, 10> names = {"SegmentID", "TriggerSource", "Channel1", "Channel2",
                                                     "Channel3", "Channel4", "Channel5", "Channel6", "Channel7",
                                                     "Channel8"};
                std::array<std::size_t, 10> ranks = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
                std::vector<std::size_t> sizes = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
                TestWriter writer(fileName, names, ranks, sizes);

                std::vector<int> col1;
                std::vector<int> col2;
                std::vector<float> col3;
                std::vector<float> col4;
                std::vector<float> col5;
                std::vector<float> col6;
                std::vector<float> col7;
                std::vector<float> col8;
                std::vector<float> col9;
                std::vector<float> col10;

                if (segID == 0) {
                    col1 = {-1};
                    col2 = {-1};
                    col3 = {(float) chans.Range[0]};
                    col4 = {(float) chans.Range[1]};
                    col5 = {(float) chans.Range[2]};
                    col6 = {(float) chans.Range[3]};
                    col7 = {(float) chans.Range[4]};
                    col8 = {(float) chans.Range[5]};
                    col9 = {(float) chans.Range[6]};
                    col10 = {(float) chans.Range[7]};
                    writer.save(col1, col2, col3, col4, col5, col6, col7, col8, col9, col10);
                    col3 = {(float) chans.DCOffset[0]};
                    col4 = {(float) chans.DCOffset[1]};
                    col5 = {(float) chans.DCOffset[2]};
                    col6 = {(float) chans.DCOffset[3]};
                    col7 = {(float) chans.DCOffset[4]};
                    col8 = {(float) chans.DCOffset[5]};
                    col9 = {(float) chans.DCOffset[6]};
                    col10 = {(float) chans.DCOffset[7]};
                    writer.save(col1, col2, col3, col4, col5, col6, col7, col8, col9, col10);
                }

                for (int i = 0; i < segSize; i ++) {
                    col1 = {segID};
                    if (triggerSource == 0) // force trigger = 9
                        chanTrig = 9;
                    else if (triggerSource == -1) // ext trigger = 0
                        chanTrig = 0;
                    else
                        chanTrig = triggerSource; // channel trigger
                    if (i != 0) {
                        col2 = {chanTrig}; // channel triggered stuff
                        col3 = {chans.Buffers[0][i]};
                        col4 = {chans.Buffers[1][i]};
                        col5 = {chans.Buffers[2][i]};
                        col6 = {chans.Buffers[3][i]};
                        col7 = {chans.Buffers[4][i]};
                        col8 = {chans.Buffers[5][i]};
                        col9 = {chans.Buffers[6][i]};
                        col10 = {chans.Buffers[7][i]};
                        writer.save(col1, col2, col3, col4, col5, col6, col7, col8, col9, col10);
                    }
                }
                break;
        }
    }

    void InitFileDisplay(const std::string& fileName, FileDisplayManager& fileDisplay) {
        // i would actaully move all the file open stuff to your main loop? or to a different function and then have a separate function for flipping through segments to make it faster &&&
        using ReaderTypes = SBC::BinaryFormat::Tools::STDVectorColumnTypes<int, int, float, float, float, float, float, float, float, float>;
        using ColNames = SBC::BinaryFormat::Tools::ColumnNames<"SegmentID", "TriggerSource", "Channel1", "Channel2",
                "Channel3", "Channel4", "Channel5", "Channel6", "Channel7",
                "Channel8">;
        // some error checking perhaps?
        SBC::BinaryFormat::DynamicStreamer<ReaderTypes, ColNames> streamer(fileName);
        fileDisplay.segments = streamer.get<"SegmentID">();
        fileDisplay.channels[0] = streamer.get<"Channel1">();
        fileDisplay.channels[1] = streamer.get<"Channel2">();
        fileDisplay.channels[2] = streamer.get<"Channel3">();
        fileDisplay.channels[3] = streamer.get<"Channel4">();
        fileDisplay.channels[4] = streamer.get<"Channel5">();
        fileDisplay.channels[5] = streamer.get<"Channel6">();
        fileDisplay.channels[6] = streamer.get<"Channel7">();
        fileDisplay.channels[7] = streamer.get<"Channel8">();


    }

    void DisplayFile(FileDisplayManager fileDisplay, ChannelControls& chans, int& segID, int segSize) {
        if (segID > fileDisplay.segments[fileDisplay.segments.size()-1])
            segID = 0;
        else if (segID < 0)
            segID = fileDisplay.segments[fileDisplay.segments.size()-1];

        int bufCounter = 0;
        int start = 2 + segID * segSize;
        int end = start + segSize;
        for (int i = 0; i < 8; i++) {
            chans.Buffers[i] = new float[segSize];
        }

        for (int i = start; i < end; i++) {
            for (int j = 0; j < 8; j++) {
                chans.Buffers[j][bufCounter] = fileDisplay.channels[j][i]; // check to make sure this works
            }
            bufCounter += 1;

        }
    }
}