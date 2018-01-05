#include "fileparser.h"

using namespace std;

void FileParser::readORCA()
{
    std::string tempString;
    QRegExp rx("", Qt::CaseInsensitive, QRegExp::RegExp2);

    while (1) {
        Molecule *molecule = new Molecule();

        // GOAL TO MATCH -
        //        ---------------------------------
        //        CARTESIAN COORDINATES (ANGSTROEM)
        //        ---------------------------------
        //          N     1.641610   -0.243155    1.175097
        //          C     2.719261   -0.702755    0.549942
        //          C     2.620982   -0.629038   -0.867321
        //          H     3.602940   -1.075919    1.062705
        //          N     1.452945   -0.127890   -1.286957
        //          H     3.405318   -0.967835   -1.537305
        //          N    -1.204954    0.202179    1.259883
        getline(infile, tempString);
        while (tempString.find("CARTESIAN COORDINATES (ANGSTROEM)") == string::npos && infile.eof() == false) {
            getline(infile, tempString);
        }
        if (infile.eof())
            break;
#ifdef QT_DEBUG
        std::cout << "readORCA: 'CARTESIAN COORDINATES (ANGSTROEM)' found.\n";
#endif

        // Geometry is reported in Angstroms
        myUnits = Angstrom;

        // Read in atom information
        rx.setPattern("(?:\\s*)(\\w+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s*)");
        while (1) {
            getline(infile, tempString);
            if (rx.exactMatch(tempString.c_str()) == true) {
                AtomEntry *atom = new AtomEntry;
                atom->Label = rx.cap(1);
                atom->x = rx.cap(2).toDouble();
                atom->y = rx.cap(3).toDouble();
                atom->z = rx.cap(4).toDouble();
                molecule->addAtom(atom);
#ifdef QT_DEBUG
                std::cout 	<< std::setw(5) << atom->Label.toStdString()
                        << " " << std::setw(16) << std::setprecision(10) << atom->x
                        << " " << std::setw(16) << std::setprecision(10) << atom->y
                        << " " << std::setw(16) << std::setprecision(10) << atom->z << std::endl;
#endif
            }
            else if(tempString.size()==0) {
                myMoleculeList.push_back(molecule);
                break;
            }
        }
    }
}
