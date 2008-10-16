SOURCES = ../src/angle.cpp  \ 
../src/anglemarker.cpp  \ 
../src/arrow.cpp  \ 
../src/atom.cpp  \ 
../src/bond.cpp  \ 
../src/drawingcanvas.cpp  \ 
../src/drawingcanvas_mouseevents.cpp  \ 
../src/drawingdisplay.cpp  \ 
../src/drawinginfo.cpp  \ 
../src/error.cpp  \ 
../src/fileparser.cpp  \ 
../src/fileparser_file11.cpp  \ 
../src/fileparser_gamess.cpp  \ 
../src/fileparser_psi3.cpp  \ 
../src/fileparser_qchem31.cpp  \ 
../src/fileparser_xyz.cpp  \ 
../src/filesavedialog.cpp  \ 
../src/label.cpp  \ 
../src/main.cpp  \ 
../src/mainwindow.cpp  \ 
../src/mainwindow_actions.cpp  \ 
../src/mainwindow_filesave.cpp  \ 
../src/mainwindow_toolbar.cpp  \ 
../src/mainwindow_toolbox.cpp  \ 
../src/mainwindow_update_menus.cpp  \ 
../src/molecule.cpp  \ 
../src/splashscreen.cpp  \ 
../src/undo_delete.cpp
HEADERS = ../src/angle.h  \ 
../src/anglemarker.h  \ 
../src/arrow.h  \ 
../src/atom.h  \ 
../src/bond.h  \ 
../src/defines.h  \ 
../src/drawingcanvas.h  \ 
../src/drawingdisplay.h  \ 
../src/drawinginfo.h  \ 
../src/error.h  \ 
../src/fileparser.h  \ 
../src/filesavedialog.h  \ 
../src/label.h  \ 
../src/mainwindow.h  \ 
../src/molecule.h  \ 
../src/splashscreen.h  \ 
../src/undo_delete.h
RESOURCES = ../src/chemvp.qrc
CONFIG += release static
QMAKE_CXXFLAGS_DEBUG = " -O0 -g"
QT += svg
