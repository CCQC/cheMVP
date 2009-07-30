#include "fileparser.h"

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

void FileParser::readFile11()
{
    std::string tempString;
    int numAtoms;
    
    QRegExp rx("", Qt::CaseInsensitive, QRegExp::RegExp2);
    
    while (1) {
        Molecule *molecule = new Molecule();
        numAtoms = 0;
        
        // GOAL TO MATCH -
        //6-31G** SCF H2O Optimization                                SCF       FIRST   
        //    3      -76.0172965718
        //        8.0000000000        0.0000000000        0.0000000000       -0.1315101651
        //        1.0000000000       -0.0000000000        1.4799409967        1.0435808775
        //        1.0000000000       -0.0000000000       -1.4799409967        1.0435808775
        //                            0.0000000000        0.0000000000       -0.0699176544
        //                            0.0000000000        0.0417341586        0.0349588272
        //                            0.0000000000       -0.0417341586        0.0349588272

        getline(infile, tempString);
        if (infile.eof()) 
            break;
        // molecule->setComment(QString(tempString.c_str()));
        
        // Match number of atoms and optional total energy
        getline(infile, tempString);
        rx.setPattern("\\s*(\\d+)\\s*(-\\d+\\.\\d+).*");
        if (rx.exactMatch(tempString.c_str()) == true) {
            numAtoms = rx.cap(1).toInt();
            molecule->setComment(rx.cap(2));
        } else {
            error("Ill formed file11.", __FILE__, __LINE__);
            return;
        }
        
        // file11 is reported in Angstroms
        myUnits = Bohr;
        
#ifdef QT_DEBUG
        std::cout << "Found a molecule with " << numAtoms << " atom." << std::endl;
#endif
        
        // Read in atom information
        rx.setPattern("(?:\\s*)(\\d+.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s*)");
        for (int i=0; i<numAtoms; ++i) {
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

#ifdef QT_DEBUG
                std::cout 	<< std::setw(5) << atom->Label.toStdString() 
                        << " " << std::setw(16) << std::setprecision(10) << atom->x
                        << " " << std::setw(16) << std::setprecision(10) << atom->y
                        << " " << std::setw(16) << std::setprecision(10) << atom->z << std::endl;
#endif
                molecule->addAtom(atom);
            }
        }
        
        for (int i=0; i<numAtoms; ++i) {
            getline(infile, tempString);     // Move past the gradients
        }
        if (numAtoms) {
            myMoleculeList.push_back(molecule);
#ifdef QT_DEBUG
            std::cout << "Adding molecule to the list" << std::endl;
#endif
        }
    }
}
