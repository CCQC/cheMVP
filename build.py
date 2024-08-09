#!/usr/bin/env python3
import os
import sys
from glob import glob
from subprocess import check_call

filename = "cheMVP.pro"
N_PROCS = os.cpu_count()

os.makedirs("build", exist_ok=True)
os.chdir("build")

sections = {
    "SOURCES": "cpp",
    "HEADERS": "h",
    "RESOURCES": "qrc",
}

out = "\n".join(
    f"{section} = " + "  \\ \n".join(glob(f"../src/*.{ext}"))
    for section, ext in sections.items()
)

out += """
CONFIG += debug_and_release static
QT += svg printsupport
QMAKE_CXXFLAGS_DEBUG = " -O0 -g"

macx{
  ICON = ../images/icon.icns
  CONFIG += x86
  LIBS += -llapack -lblas
  QMAKE_INFO_PLIST = ../chemvp.plist
}

win32{
  RC_FILE = ../images/icon.rc
  CONFIG -= debug
  QMAKE_CXXFLAGS_RELEASE -= -mthreads
  QMAKE_LFLAGS_RELEASE -= -mthreads
}"""

with open(filename, "w") as f:
    f.write(out)

qmake_flags = "-spec macx-g++" if len(sys.argv) > 1 and sys.argv[1] == "mac" else ""

check_call(f"qmake {qmake_flags} {filename}".split())
check_call(f"time nice make -j {N_PROCS}".split())
