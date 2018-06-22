#include "fileparser.h"

using namespace std;

void FileParser::readQchem31()
{
    std::string tempString;
    QRegExp rx("", Qt::CaseInsensitive, QRegExp::RegExp2);

    while (1) {
        Molecule *molecule = new Molecule();

        // GOAL TO MATCH -
        // Optimization Cycle:   1
        //
        //                     Coordinates (Angstroms)
        //   ATOM              X           Y           Z
        //  1  H           6.264255   -1.214463    0.001562
        //  2  N           0.992577    0.405079   -0.000713
        //  3  C           3.640581    0.699759   -0.000285
        //  4  H           5.692879    1.292403   -0.000233
        //  5  N           1.103169   -1.927005    0.000687
        //  6  C          -3.986741   -0.820146    0.000147
        //  7  C          -4.603474    0.385503    0.001374
        //  8  N           4.158387   -1.540250    0.001279
        //  9  C           5.256835   -0.819403    0.001066
        // 10  H          -5.684216    0.487120    0.002358
        // 11  N           2.932990    1.843103   -0.001138
        // 12  H          -4.383326    2.467645    0.002362
        // 13  H           0.959209    2.465806   -0.001902
        // 14  H           1.658401   -2.773307    0.001857
        // 15  C           1.728674   -0.733977    0.000168
        // 16  N           5.011372    0.541687    0.000124
        // 17  H          -4.452049   -2.729414    0.878462
        // 18  C           3.136252   -0.602051    0.000394
        // 19  H          -0.845397    0.417252   -0.001289
        // 20  H          -4.453040   -2.728702   -0.879157
        // 21  H          -5.802705   -1.976283    0.000726
        // 22  C          -4.719039   -2.131115    0.000042
        // 23  C           1.619390    1.601695   -0.001312
        // 24  H           0.079495   -1.982939    0.000872
        // 25  N          -3.902148    1.574898    0.001480
        // 26  C          -2.511246    1.648434    0.000286
        // 27  O          -1.910441    2.717732    0.000430
        // 28  N          -1.895228    0.410754   -0.001007
        // 29  C          -2.522920   -0.833703   -0.001123
        // 30  O          -1.848461   -1.874351   -0.002325

        getline(infile, tempString);
        while (tempString.find("Optimization Cycle:") == string::npos && infile.eof() == false) {
            getline(infile, tempString);
        }
        if (infile.eof()) {
            break;
        }
#ifdef QT_DEBUG
        std::cout << "readQchem31: 'Optimization Cycle:' found.\n";
#endif

        getline(infile, tempString);
        getline(infile, tempString);
        getline(infile, tempString);

        // qchem31 is reported in Angstroms
        myUnits = Angstrom;

        // Read in atom information
        rx.setPattern("(?:\\s*)(?:\\d+)(?:\\s+)(\\w+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)"
                      "(?:\\s+)(-?\\d+\\.\\d+)");
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
                std::cout << std::setw(5) << atom->Label.toStdString() << " " << std::setw(16)
                          << std::setprecision(10) << atom->x << " " << std::setw(16)
                          << std::setprecision(10) << atom->y << " " << std::setw(16)
                          << std::setprecision(10) << atom->z << std::endl;
#endif
            } else {
                myMoleculeList.push_back(molecule);
                break;
            }
        }
    }
}
