MouseSim
========

[![Build status](https://ci.appveyor.com/api/projects/status/s8i0no1qxchy00qm?svg=true)](https://ci.appveyor.com/project/axelstudios/mousesim)

MouseSim is a simple tool for Windows to simulate mouse movement and avoid computer inactivity - It works by using the Windows API to move the mouse cursor zero pixels every second.  The effect is impossible to detect by the user, and it is ideal for computers with domain policies that lock the system after a set period of inactivity.

#### Build Instructions
MouseSim requires at least CMake 3, Qt 5.8 (dynamic or static), and NSIS with the [KillProcWMI plugin](https://github.com/axelstudios/MouseSim/wiki) for packaging.

To build Qt 5.8.0 statically, launch the Visual Studio 2015 x64 command prompt in a Qt source directory:

```
configure.bat -platform win32-msvc2015 -opensource -debug-and-release -static -static-runtime -opengl desktop -no-angle -qt-libpng -no-libjpeg -qt-freetype -qt-harfbuzz -skip enginio -no-compile-examples -nomake examples -nomake tests -nomake tools -mp -confirm-license -prefix "C:/Qt/5.8.0.static" -no-qml-debug -skip qtwebengine -skip qtwebkit
```

Add the KillProcWMI plugin to your NSIS installation to allow NSIS to kill the 64-bit process upon uninstallation.
