#include "fileparser.h"

using namespace std;

static char *atomic_labels[]= 
{"X","H","HE","LI","BE","B","C","N","O","F","NE","NA","MG","AL","SI",
"P","S","CL","AR","K","CA","SC","TI","V","CR","MN","FE","CO","NI",
"CU","ZN","GA","GE","AS","SE","BR","KR","RB","SR","Y","ZR","NB","MO",
"TC","RU","RH","PD","AG","CD","IN","SN","SB","TE","I","XE","CS","BA",
"LA","CE","PR","ND","PM","SM","EU","GD","TB","DY","HO","ER","TM","YB",
"LU","HF","TA","W","RE","OS","IR","PT","AU","HG","TL","PB","BI","PO",
"AT","RN","FR","RA","AC","TH","PA","U","NP","PU","AM","CM","BK","CF",
"ES","FM","MD","NO","LR","RF","DB","SG","BH","HS","MT","DS","RG",
  "UUB","UUT","UUQ","UUP","UUH","UUS","UUO"};
  
void FileParser::readPsi3()
{    
    std::string tempString;    
    QRegExp rx("", Qt::CaseInsensitive, QRegExp::RegExp2);
    
    while (1) {
        Molecule *molecule = new Molecule();
        
        // GOAL TO MATCH -
        // New Cartesian Geometry in a.u.
        //           8.0   0.0000000000   0.0000000000  -0.1222529019
        //           1.0   0.0000000000   1.3998179435   0.9701211351
        //           1.0   0.0000000000  -1.3998179435   0.9701211351
        getline(infile, tempString);
        while (tempString.find("New Cartesian Geometry in a.u.") == string::npos && infile.eof() == false) {
            getline(infile, tempString);
        }
        if (infile.eof()) 
            break;
        #ifdef QT_DEBUG        
        std::cout << "readPsi3: 'New Cartesian Geometry in a.u.' found.\n";
        #endif
        
        // file11 is reported in Angstroms
        myUnits = Bohr;
        
        // Read in atom information
        rx.setPattern("(?:\\s*)(\\d+.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s*)");
        while (1) {
            getline(infile, tempString);
            if (rx.exactMatch(tempString.c_str()) == true) {
                AtomEntry *atom = new AtomEntry;
                atom->Label = atomic_labels[int(rx.cap(1).toDouble())];
                atom->x = rx.cap(2).toDouble();
                atom->y = rx.cap(3).toDouble();
                atom->z = rx.cap(4).toDouble();
                atom->x *= BOHR_TO_ANG;
				atom->y *= BOHR_TO_ANG;
				atom->z *= BOHR_TO_ANG;
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
    }
}
