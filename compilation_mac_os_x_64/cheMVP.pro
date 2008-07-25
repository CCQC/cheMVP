SOURCES = ../src/angle.cpp  \ 
../src/anglemarker.cpp  \ 
../src/arrow.cpp  \ 
../src/atom.cpp  \ 
../src/bond.cpp  \ 
../src/drawingcanvas.cpp  \ 
../src/drawinginfo.cpp  \ 
../src/error.cpp  \ 
../src/fileparser.cpp  \ 
../src/fileparser_file11.cpp  \ 
../src/fileparser_xyz.cpp  \ 
../src/filesavedialog.cpp  \ 
../src/label.cpp  \ 
../src/main.cpp  \ 
../src/mainwindow.cpp  \ 
../src/mainwindow_filesave.cpp  \ 
../src/molecule.cpp
HEADERS = ../src/angle.h  \ 
../src/anglemarker.h  \ 
../src/arrow.h  \ 
../src/atom.h  \ 
../src/bond.h  \ 
../src/defines.h  \ 
../src/drawingcanvas.h  \ 
../src/drawinginfo.h  \ 
../src/error.h  \ 
../src/fileparser.h  \ 
../src/filesavedialog.h  \ 
../src/label.h  \ 
../src/mainwindow.h  \ 
../src/molecule.h
RESOURCES = ../src/chemvp.qrc
CONFIG += qt warn_on debug
QMAKE_CXXFLAGS_DEBUG = "-O0 -g"
QT += svg
