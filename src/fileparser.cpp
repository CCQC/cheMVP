#include "fileparser.h"

FileParser::FileParser(QString instring):
	myFileName(instring),
	myUnits(Angstrom),
	currentGeometry(0)
{
}


FileParser::~FileParser()
{
	
}


void FileParser::determineFileType()
{
    if (myFileName.contains("file11", Qt::CaseInsensitive))
        fileType = FILE11;
	else
	    fileType = XYZ;
}


void FileParser::readFile()
{
	if(myFileName.isEmpty()) return;
	determineFileType();
	foreach(Molecule *molecule, myMoleculeList){
		delete molecule;
	}
	myMoleculeList.clear();
	
	switch(fileType){
		case XYZ:
			readXYZ();
			break;
		case FILE11:
            readFile11();
            break;
		default:
		;
	}
}