#include "fileparser.h"

using namespace std;

void FileParser::readNWChem()
{    
    std::string tempString;    
    QRegExp rx("", Qt::CaseInsensitive, QRegExp::RegExp2);
    
    while (1) {
        Molecule *molecule = new Molecule();
        // GOAL TO MATCH -
		//	        --------      
		//	        Step   0      
		//	        --------       
		//	    
		//	    
		//	                       Geometry "geometry" -> "geometry"
		//	                       ---------------------------------
		//	
		//	Output coordinates in angstroms (scale by  1.889725989 to convert to a.u.)
		//	
		//	No.       Tag          Charge          X              Y              Z
		//	---- ---------------- ---------- -------------- -------------- --------------
		//	  1 Fe                  26.0000    -0.22869670     0.31109896    -0.00003072
		//	  2 N                    7.0000    -2.41291156     0.06870095     0.00120839
		//	  3 N                    7.0000    -0.27336714     0.52308268     2.17933716
		//	  4 H                    1.0000    -0.41056709    -0.40789333     2.59764604
		//	  5 H                    1.0000     0.60009756     0.92399733     2.54645141
		//	  6 N                    7.0000    -0.27622082     0.52392375    -2.17925576
		//	  7 H                    1.0000    -0.41646960    -0.40637152    -2.59801380
		//	  8 H                    1.0000    -1.03530848     1.15173474    -2.47756534
		//	  9 N                    7.0000     1.96210712     0.25481672    -0.00154756
		//	 10 H                    1.0000     2.30942597    -0.25794214     0.81985399
		//	 11 H                    1.0000     2.30826247    -0.25897245    -0.82279696
		//	 12 S                   16.0000    -0.30655592    -2.14024426     0.00012199
        getline(infile, tempString);
        rx.setPattern("Step\\s+\\d");
        while (rx.indexIn(tempString.c_str()) == -1 && infile.eof() == false) {
            getline(infile, tempString);
        }
        if (infile.eof()) 
            break;
        #ifdef QT_DEBUG        
        std::cout << "readNWChem: 'Step XX' found" << std::endl;
        #endif
        
        // Assume bohr for now...
        myUnits = Bohr;
        
        // Read in atom information
        rx.setPattern("(\\w+)\\s+\\d+\\.\\d+\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)\\s+(-?\\d+\\.\\d+)");
        while (1) {
            getline(infile, tempString);
            if (rx.indexIn(tempString.c_str()) != -1) {
                AtomEntry *atom = new AtomEntry;
                atom->Label = rx.cap(1);
                atom->x = rx.cap(2).toDouble();
                atom->y = rx.cap(3).toDouble();
                atom->z = rx.cap(4).toDouble();
                if(myUnits==Bohr){
	                atom->x *= BOHR_TO_ANG;
					atom->y *= BOHR_TO_ANG;
					atom->z *= BOHR_TO_ANG;
                }
                molecule->addAtom(atom);
                #ifdef QT_DEBUG
                std::cout 	<< std::setw(5) << atom->Label.toStdString() 
        			<< " " << std::setw(16) << std::setprecision(10) << atom->x 
        			<< " " << std::setw(16) << std::setprecision(10) << atom->y 
        			<< " " << std::setw(16) << std::setprecision(10) << atom->z << std::endl; 
                #endif
            }else if(tempString.find("Atomic Mass") != string::npos){
                myMoleculeList.push_back(molecule);
                break;
	        }else if(tempString.find("in angstroms") != string::npos){
	            myUnits = Angstrom;
	        }
        }
    }
}
