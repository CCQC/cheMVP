#include "fileparser.h"

void FileParser::readXYZ()
{
	std::string tempString;
	int numAtoms;

	QRegExp rx("(\\d+)(?:\\s*)(bohr|au)?", Qt::CaseInsensitive, QRegExp::RegExp2);
	
	while( !infile.eof() ){
		Molecule* molecule = new Molecule();
		numAtoms = 0;
		getline(infile, tempString);
		// The first line - number of atoms, then optionally units, e.g
		// 8   bohr
		rx.setPattern("(\\d+)(?:\\s*)(bohr|au)?");
		if(rx.indexIn(tempString.c_str()) != -1){
			numAtoms = rx.cap(1).toInt();
			if(!rx.cap(2).isEmpty()){
				myUnits = Bohr;
			}else{
				myUnits = Angstrom;
			}
#ifdef QT_DEBUG
	std::cout << "Found a molecule with " << numAtoms << " atoms, expressed in " <<
				(myUnits == Angstrom ? "Angstrom" : "Bohr") << std::endl;
#endif
		// Blank lines are acceptable
		}else if(tempString.size()){
			QString errorMessage = "Unrecognized XYZ format in " + myFileName;
			errorMessage += "\nI don't understand \n\n";
			errorMessage += tempString.c_str();
			errorMessage += "\nThe format should be \n num_atoms [(bohr|au)]";
			error(errorMessage,__FILE__,__LINE__);
			return;
		}
		// This should be a comment line. Grab it and store it.
		getline(infile, tempString);
		molecule->setComment(QString(tempString.c_str()));
		// Look for something like
		// atom_symbol double double double
		// with the constraint that atom_symbol is the exact symbol used in the periodic table
		rx.setPattern("(?:\\s*)([A-Z](?:[a-z])?)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s*)");
		for(int i=0; i<numAtoms; ++i){
			AtomEntry *atom = new AtomEntry;
			getline(infile, tempString);
			if(rx.exactMatch(tempString.c_str())){
				atom->Label = rx.cap(1);
				atom->x 	= rx.cap(2).toDouble();
				atom->y 	= rx.cap(3).toDouble();
				atom->z 	= rx.cap(4).toDouble();
				if(myUnits == Bohr){
					atom->x *= BOHR_TO_ANG;
					atom->y *= BOHR_TO_ANG;
					atom->z *= BOHR_TO_ANG;
				}
#ifdef QT_DEBUG
			std::cout 	<< std::setw(5) << atom->Label.toStdString() 
						<< " " << std::setw(16) << std::setprecision(10) << atom->x 
						<< " " << std::setw(16) << std::setprecision(10) << atom->y 
						<< " " << std::setw(16) << std::setprecision(10) << atom->z << std::endl; 
#endif
				molecule->addAtom(atom);
			}else{
				QString errorMessage = "Unrecognized XYZ format in " + myFileName;
				errorMessage += "\nI don't understand \n\n";
				errorMessage += tempString.c_str();
				errorMessage += "\nThe format should be \n atom_symbol x y z";
				errorMessage += "\n where atom_label is the atom_symbol is the symbol used in the periodic table ";
				errorMessage += "and x, y and z are floating point numbers";
				error(errorMessage,__FILE__,__LINE__);
				myMoleculeList.clear();
				return;
			}
		}
		if(numAtoms){
			myMoleculeList.push_back(molecule);
#ifdef QT_DEBUG
			std::cout << "Adding molecule to the list" << std::endl;
#endif
			
		}
	}
}
