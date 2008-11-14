#ifndef FILEPARSER_H_
#define FILEPARSER_H_

#include <QString>
#include <vector>
#include <fstream>
#include <QRegExp>
#include <limits>
#include <string>
#include "molecule.h"
#include "defines.h"
#include "error.h"

#ifdef QT_DEBUG
	#include <iostream>
	#include <iomanip>
#endif


class FileParser
{
	enum FileType {UNKNOWN, XYZ, PSI3, FILE11, QCHEM3_1, GAMESS, ACES2, NWCHEM};
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
    void readNWChem();
    void readQchem31();
        
    std::ifstream infile;
	FileType fileType;
	UnitsType myUnits;
	QString myFileName;
	int currentGeometry;
	QList<Molecule*> myMoleculeList;
};
#endif /*FILEPARSER_H_*/
