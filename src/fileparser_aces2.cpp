#include "fileparser.h"

using namespace std;

void FileParser::readACES2()
{    
    std::string tempString;    
    QRegExp rx("", Qt::CaseInsensitive, QRegExp::RegExp2);
    bool isFindif=false; //We only want the first geometry for finite difference frequency computations
    while (1) {
        Molecule *molecule = new Molecule();
        
        // GOAL TO MATCH -
        //  Symbol    Number           X              Y              Z    
        // ----------------------------------------------------------------
        //     C         6         0.00000000     0.00000000     1.13729393
        //     X         0         1.88972652     0.00000000     1.13729393
        //     C         6         0.00000000     0.00000000    -1.13729393
        //     H         1         0.00000000     0.00000000     3.14435975
        //     X         0         1.88972652     0.00000000    -1.13729393
        //     H         1         0.00000000     0.00000000    -3.14435975
        // ----------------------------------------------------------------

        
        getline(infile, tempString);
        while (tempString.find("Symbol    Number") == string::npos && infile.eof() == false) {
            if (tempString.find("IVIB             FINDIF") != string::npos) isFindif=true;
            getline(infile, tempString);
        }
        if (infile.eof()) 
            break;
        #ifdef QT_DEBUG        
        std::cout << "readAces: 'Symbol    Number' found.\n";
        #endif
        // Skip line containing just dashes, just before the coordinates themselves
        getline(infile, tempString);
        
        // ACES geometry is reported in Bohr
        myUnits = Bohr;
        
        // Read in atom information
        rx.setPattern("(?:\\s*)(\\w+)(?:\\s+)(?:\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)");
        while (1) {
            getline(infile, tempString);
            if (rx.exactMatch(tempString.c_str()) == true) {
                AtomEntry *atom = new AtomEntry;
                QString symbol = rx.cap(1);
                // ACES capitalizes EVERYTHING, make sure the symbol is correct
                if(symbol.size()>1) symbol[1] = symbol[1].toLower();
                atom->Label = symbol;
                atom->x = rx.cap(2).toDouble()*BOHR_TO_ANG;
                atom->y = rx.cap(3).toDouble()*BOHR_TO_ANG;
                atom->z = rx.cap(4).toDouble()*BOHR_TO_ANG;
                molecule->addAtom(atom);
                #ifdef QT_DEBUG
                std::cout 	<< std::setw(5) << atom->Label.toStdString() 
        			<< " " << std::setw(16) << std::setprecision(10) << atom->x 
        			<< " " << std::setw(16) << std::setprecision(10) << atom->y 
        			<< " " << std::setw(16) << std::setprecision(10) << atom->z << std::endl; 
                #endif
            }
            else {
                myMoleculeList.push_back(molecule);
                break;
            }
        }
        if(isFindif) break;
    }
}