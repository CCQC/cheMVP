#ifndef FILEPARSER_H_
#define FILEPARSER_H_

#include <QString>
#include <QRegExp>

#include <ios>
#include <limits>
#include <string>
#include <vector>
#include <fstream>

#include "molecule.h"
#include "defines.h"
#include "error.h"

#ifdef QT_DEBUG
	#include <iostream>
	#include <iomanip>
#endif

class FileParser
{
	enum FileType {UNKNOWN, XYZ, PSI3, FILE11, QCHEM3_1, GAMESS, ORCA, ACES2, NWCHEM, MOLPRO};
	enum UnitsType {Angstrom, Bohr};

public:
	FileParser(QString infile);
	~FileParser();
	
	Molecule* molecule() {return myMoleculeList[currentGeometry];}

	int numMolecules() {return myMoleculeList.size();}
    int current() const { return currentGeometry; }
    void setCurrent(int curr) { currentGeometry = curr; }
	void setFileName(const QString name) {myFileName = name;}
	
	const QString& fileName() {return myFileName;}

	void readFile();
	
protected:
	void determineFileType();
	void readXYZ();
    void readFile11();
    void readPsi3();
    void readGamess();
    void readACES2();
    void readORCA();
    void readNWChem();
    void readQchem31();
    void readMolpro();
        
    std::ifstream infile;
	FileType fileType;
	UnitsType myUnits;
	QString myFileName;
	int currentGeometry;
	QList<Molecule*> myMoleculeList;
};

#endif /*FILEPARSER_H_*/