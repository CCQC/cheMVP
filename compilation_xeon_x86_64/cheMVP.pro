SOURCES = ../src/angle.cpp  \ 
../src/anglemarker.cpp  \ 
../src/arrow.cpp  \ 
../src/bond.cpp  \ 
../src/drawinginfo.cpp  \ 
../src/error.cpp  \ 
../src/fileparser.cpp  \ 
../src/filesavedialog.cpp  \ 
../src/label.cpp  \ 
../src/main.cpp  \ 
../src/molecule.cpp  \ 
../src/fileparser_xyz.cpp  \ 
../src/atom.cpp  \ 
../src/drawingcanvas.cpp  \ 
../src/mainwindow.cpp  \ 
../src/mainwindow_filesave.cpp
HEADERS = ../src/angle.h  \ 
../src/anglemarker.h  \ 
../src/arrow.h  \ 
../src/bond.h  \ 
../src/defines.h  \ 
../src/error.h  \ 
../src/filesavedialog.h  \ 
../src/label.h  \ 
../src/molecule.h  \ 
../src/fileparser.h  \ 
../src/atom.h  \ 
../src/drawingcanvas.h  \ 
../src/drawinginfo.h  \ 
../src/mainwindow.h
RESOURCES = ../src/chemvp.qrc
CONFIG += qt warn_on debug
QMAKE_CXXFLAGS_DEBUG = "-O0 -g"
QT += svg
