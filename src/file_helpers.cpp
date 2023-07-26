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
                            << "Segment ID,Timestamp,Trigger Source,Channel1,Channel2,Channel3,Channel4,Channel5,Channel6.Channel7,Channel8"
                            << std::endl;
                    dataFile << "-1,-1,-1," << chans.Range[0] << "," << chans.Range[1] << "," << chans.Range[2] << ","
                             << chans.Range[3] << "," << chans.Range[4] << "," << chans.Range[5] << ","
                             << chans.Range[6] << "," << chans.Range[7] << std::endl;
                    dataFile << "-1,-1,-1," << chans.DCOffset[0] << "," << chans.DCOffset[1] << ","
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
                        dataFile << segID << "," << chans.CurrentTimeStamp << "," << chanTrig << "," << chans.Buffers[0][i] << "," << chans.Buffers[1][i] << ","
                                 << chans.Buffers[2][i] << "," << chans.Buffers[3][i] << "," << chans.Buffers[4][i] << "," <<
                                 chans.Buffers[5][i] << "," << chans.Buffers[6][i] << "," << chans.Buffers[7][i] << std::endl;
                    }
                }

                dataFile.close();
                break;

            case RunControls::sbc:
                using TestWriter = SBC::BinaryFormat::DynamicWriter<int, double, int, double, double, double, double, double, double, double, double>;
                std::array<std::string, 11> names = {"Segment ID", "Timestamp", "Trigger Source", "Channel1", "Channel2",
                                                     "Channel3", "Channel4", "Channel5", "Channel6", "Channel7",
                                                     "Channel8"};
                std::array<std::size_t, 11> ranks = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
                std::vector<std::size_t> sizes = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
                TestWriter writer(fileName, names, ranks, sizes);

                std::vector<int> col0;
                std::vector<double> col1;
                std::vector<int> col2;
                std::vector<double> col3;
                std::vector<double> col4;
                std::vector<double> col5;
                std::vector<double> col6;
                std::vector<double> col7;
                std::vector<double> col8;
                std::vector<double> col9;
                std::vector<double> col10;

                if (segID == 0) {
                    col0 = {-1};
                    col1 = {-1.0};
                    col2 = {-1};
                    col3 = {(double) chans.Range[0]};
                    col4 = {(double) chans.Range[1]};
                    col5 = {(double) chans.Range[2]};
                    col6 = {(double) chans.Range[3]};
                    col7 = {(double) chans.Range[4]};
                    col8 = {(double) chans.Range[5]};
                    col9 = {(double) chans.Range[6]};
                    col10 = {(double) chans.Range[7]};
                    writer.save(col0, col1, col2, col3, col4, col5, col6, col7, col8, col9, col10);
                    col3 = {(double) chans.DCOffset[0]};
                    col4 = {(double) chans.DCOffset[1]};
                    col5 = {(double) chans.DCOffset[2]};
                    col6 = {(double) chans.DCOffset[3]};
                    col7 = {(double) chans.DCOffset[4]};
                    col8 = {(double) chans.DCOffset[5]};
                    col9 = {(double) chans.DCOffset[6]};
                    col10 = {(double) chans.DCOffset[7]};
                    writer.save(col0, col1, col2, col3, col4, col5, col6, col7, col8, col9, col10);
                }

                for (int i = 0; i < segSize; i ++) {
                    col0 = {segID};
                    col1 = {chans.CurrentTimeStamp};
                    if (triggerSource == 0) // force trigger = 9
                        chanTrig = 9;
                    else if (triggerSource == -1) // ext trigger = 0
                        chanTrig = 0;
                    else
                        chanTrig = triggerSource; // channel trigger
                    if (i != 0) {
                        col2 = {chanTrig}; // channel triggered stuff
                        col3 = {(double)chans.Buffers[0][i]};
                        col4 = {(double)chans.Buffers[1][i]};
                        col5 = {(double)chans.Buffers[2][i]};
                        col6 = {(double)chans.Buffers[3][i]};
                        col7 = {(double)chans.Buffers[4][i]};
                        col8 = {(double)chans.Buffers[5][i]};
                        col9 = {(double)chans.Buffers[6][i]};
                        col10 = {(double)chans.Buffers[7][i]};
                        writer.save(col0, col1, col2, col3, col4, col5, col6, col7, col8, col9, col10);
                    }
                }
                break;
        }
    }

    void InitFileDisplay(const std::string& fileName, FileDisplayManager& fileDisplay, RunControls& params) {
        using ReaderTypes = SBC::BinaryFormat::Tools::STDVectorColumnTypes<int, double, int, double, double, double, double, double, double, double, double>;
        using ColNames = SBC::BinaryFormat::Tools::ColumnNames<"Segment ID", "Timestamp", "Trigger Source", "Channel1", "Channel2",
                "Channel3", "Channel4", "Channel5", "Channel6", "Channel7",
                "Channel8">;
        // some error checking perhaps?
        SBC::BinaryFormat::DynamicStreamer<ReaderTypes, ColNames> streamer(fileName);
        fileDisplay.segments = streamer.get<"Segment ID">();
        fileDisplay.channels[0] = streamer.get<"Channel1">();
        fileDisplay.channels[1] = streamer.get<"Channel2">();
        fileDisplay.channels[2] = streamer.get<"Channel3">();
        fileDisplay.channels[3] = streamer.get<"Channel4">();
        fileDisplay.channels[4] = streamer.get<"Channel5">();
        fileDisplay.channels[5] = streamer.get<"Channel6">();
        fileDisplay.channels[6] = streamer.get<"Channel7">();
        fileDisplay.channels[7] = streamer.get<"Channel8">();

        params.PostTriggerSamples = (int)(fileDisplay.segments.size() - 2)/(fileDisplay.segments[fileDisplay.segments.size()-1] + 1);
        params.PreTriggerSamples = 0;

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
                chans.Buffers[j][bufCounter] = (float)fileDisplay.channels[j][i]; // check to make sure this works
            }
            bufCounter += 1;

        }
    }
}
