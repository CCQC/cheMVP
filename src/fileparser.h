#ifndef FILEPARSER_H_
#define FILEPARSER_H_

#include <QDir>
#include <QRegExp>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <fstream>
#include <ios>
#include <limits>
#include <string>
#include <vector>

#include "defines.h"
#include "error.h"
#include "molecule.h"

#ifdef QT_DEBUG
#include <iomanip>
#include <iostream>
#endif

class FileParser
{
    enum FileType { UNKNOWN, XYZ, PSI3, FILE11, QCHEM3_1, GAMESS, ORCA, ACES2, NWCHEM, MOLPRO };
    enum UnitsType { Angstrom, Bohr };

  public:
    FileParser(QString infile);
    ~FileParser();

    Molecule *molecule()
    {
        return myMoleculeList[currentGeometry];
    }

    int numMolecules()
    {
        return myMoleculeList.size();
    }
    int current() const
    {
        return currentGeometry;
    }
    void setCurrent(int curr)
    {
        currentGeometry = curr;
    }
    const QString &fileName()
    {
        return myFileName;
    }
    void setFileName(const QString name);
    void readFile();
    void serialize(QXmlStreamWriter *writer);
    static FileParser *deserialize(QXmlStreamReader *reader);

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
    QList<Molecule *> myMoleculeList;
};

#endif /*FILEPARSER_H_*/
