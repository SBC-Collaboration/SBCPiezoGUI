# SBCPiezoGUI

A user interface for acoustic digitzer signals. 

## Acknowledgements
The files in include/SBCBinaryFormat were written by Hector Hawley Herrera and can be found at [link]. Other dependencies include [links]; these do not need to be separately installed.

## Control Panel
There are four tabs on the control panel:

#### Acquisition Options
[image]
These control parameters that remain constant while the digitizer is acquiring data, including the number of samples per event to collect (both before and after a trigger) and the total number of events. Data saving is also included; you can save in .txt or .sbc.bin format, with an additional option to save whatever signals are currently being displayed on the UI (in case you didn't select Save File before acquiring and now want to save your data). 

#### Channel Options
[image]
These control channel-specific parameters, including input range in mV and DC offset as a percentage of the input range. See SBC-Piezo-Base-Code repository or the Gage INI file documentation for more info.

#### Trigger Options
[image]
These control trigger parameters, including number of triggers and parameters for each.

### Known Issues
- Currently, external trigger range is set by default to [value], so the level % is with respect to this.

## Graph Panel
[image]

Each graph corresponds to a digitizer channel. If your signal is not visible on the y-axis, double click on it to zoom to fit the data. You can also click and drag or scroll to adjust position and scale.

### Known Issues

If your data is too long (around 12-15k samples or more), you won't be able to view all data at the same time on the x-axis (you shouldn't double click to rescale or the program will throw an error as it won't have enough space to display all the data). You can still scroll/click and drag to view later samples, though, and all your desired data will still be collected and saved. I'm considering adding functionality to hide certain graphs from view or changing layout to allow for longer graphs.

## Other Issues


(to discuss)
- timestamp issue?
