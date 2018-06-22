# XCI-Cutter-Qt
XCI-Cutter Qt 5.9.4 project multiplatform.

Starting from https://github.com/Destiny1984/XCI-Cutter , I decided to port the project to Qt/c++, to use on MacOS / Linux.
At the moment this is an ALFA version, with only the cut function available (all the sanity checks implemented and no mod to original image).

To generate app redistributable, use command: <pat to macdeployqt> XCICutter.app  -verbose=3 -dmg