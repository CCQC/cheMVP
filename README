ABOUT
=====
cheMVP is free, open-source software designed to make clean, simple molecule
drawings suitable for publications and presentations.  The program is written
in C++, using the Qt library and some icons from the SVG icons project.

OUTPUT FORMATS
==============
cheMVP can generate PDF, SVG, EPS, PS, PNG and Tiff images.  The PDF, SVG and
PostScript formats are recommended, as they are vector formats, which means
that they are scalable and can be manipulated with software such as InkScape
(free) or Adobe Illustrator (not free, but very powerful).  PNG files seem to
be smaller than Tiffs, so they are probably a bit better.  The file type is
determined by the extension used in the filename.  An experimental CVP format
is being developed to save the project, allowing the user to save the project's
state and load it again.

BASIC USAGE
===========
Most buttons and settings apply only to the selected items.  In select mode,
clicking outside of the molecule and dragging a rectangle over molecule
components will select them, or clicking individual components will
select/deselect them.  To add a bond, click the "add bond" mode button, click
the first atom and drag the mouse over the second atom to be bonded and
release.

INPUT FORMATS
=============
Many input formats are supported and are automatically detected.  The only
restriction is for xyz files, which currently need to have the extension .xyz
and take the following format:

NumberOfAtoms  [bohr or au]
Comment Line
Atom1Symbol Atom1XCoordinate Atom1YCoordinate Atom1ZCoordinate
Atom2Symbol Atom2XCoordinate Atom2YCoordinate Atom2ZCoordinate
Atom3Symbol Atom3XCoordinate Atom3YCoordinate Atom3ZCoordinate
etc..

Note that multiple xyz sets can be present in a single xyz file.  In terms of
chemistry packages, the following outputs can be read, and there are no
restrictions on the filenames used:- the optional argument after the number of
atoms is only necessary if the units are atomic units.

 o Molpro (geometry optimization) 
 o GAMESS (geometry optimization)
 o NWChem (geometry optimization)
 o ORCA (single point/geometry optimization)
 o PSI3 (geometry optimization)
 o Q-Chem 3.1 
 o File11 from PSI
 o ACESII/Cfour

a save/load project format for cheMVP is in development; stay tuned, folks.  Be
careful if you've done a funky many-jobs-in-one-input type calculation and
check that the correct geometries were parsed.  Sometimes it's tricky to
distinguish the steps of a finite difference computation from steps of an
optimization. 



COMPILING
=========
If you can't find a pre-compiled binary for your OS, then to compile from
source is pretty simple.

1) Download and install Qt from http://qt.nokia.com/downloads
2) Head into the compilation directory in the chemvp source folder
3) Type 'qmake'
4) Type 'make'


WARRANTY
========
cheMVP - A molecule rendering program

Copyright Andrew C. Simmonett 2010

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Please contact me with any updates/modifications you have made, so that I can
incorporate them into the code.

AUTHORS
=======
Andy Simmonett andysim@ccc.uga.edu
Justin M. Turney jturney@ccc.uga.edu
H. Parker Shelton

LINKS
=====
A number of valuable open-source programs have been used to create this code.
They are:
Qt (GUI library)  - http://qt.nokia.com
SVG Icons (the icons)  - http://svgicons.sourceforge.net
GIT (code synchronization) - http://git-scm.com/

OTHER CONTRIBUTORS
==================
Jesse Yates - FontFormatTuple for Label serialization
