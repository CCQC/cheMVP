#include "fileparser.h"

using namespace std;

void FileParser::readMolpro()
{
    std::string tempString;
    QRegExp rx("", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp rx_done("\\s+Bond lengths.*", Qt::CaseInsensitive, QRegExp::RegExp2);
    //        GOAL TO MATCH -
    //        Convergence:                0.00000000  (line search)     0.44148917     0.14465594
    //        (total)
    //
    //        Geometry written to block  3 of record 700
    //
    //
    //        Recomputing integrals since geometry changed
    //
    //       1PROGRAM * SEWARD (Integral evaluation for generally contracted gaussian basis sets)
    //       Author: Roland Lindh, 1990
    //
    //        Geometry written to block  3 of record 700
    //
    //
    //        Point group  C1
    //
    //
    //
    //        ATOMIC COORDINATES
    //
    //        NR  ATOM    CHARGE       X              Y              Z
    //
    //          1  N       7.00    3.192403534   -0.718429097    2.097689725
    //         22  S      16.00   -0.650157643   -4.707933333   -0.263117658
    //         23  FE     26.00    0.198822354    0.010404544   -0.122326318
    //
    //        Bond lengths in Bohr (Angstrom)

    // Molpro geometries are reported in Bohr
    myUnits = Bohr;

    while (infile.eof() == false) {
        getline(infile, tempString);
        if (tempString.find("Convergence:") != string::npos) {
            Molecule *molecule = new Molecule();
#ifdef QT_DEBUG
            std::cout << "readMolpro: Convergence:' found.\n";
#endif
            if (infile.eof()) {
                break;
            }
            // Read in atom information
            rx.setPattern("(?:\\s*)(?:\\d+)(?:\\s+)(\\w+)(?:\\s+)(?:\\d+.\\d+)(?:\\s+)(-?\\d+\\."
                          "\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)");
            while (1) {
                getline(infile, tempString);
                if (rx.exactMatch(tempString.c_str()) == true) {
                    AtomEntry *atom = new AtomEntry;
                    QString symbol = rx.cap(1);
                    // Molpro capitalizes atom labels, make sure the symbol is correct
                    if (symbol.size() > 1) {
                        symbol[1] = symbol[1].toLower();
                    }
                    atom->Label = symbol;
                    atom->x = BOHR_TO_ANG * rx.cap(2).toDouble();
                    atom->y = BOHR_TO_ANG * rx.cap(3).toDouble();
                    atom->z = BOHR_TO_ANG * rx.cap(4).toDouble();
                    molecule->addAtom(atom);
#ifdef QT_DEBUG
                    std::cout << std::setw(5) << atom->Label.toStdString() << " " << std::setw(16)
                              << std::setprecision(10) << atom->x << " " << std::setw(16)
                              << std::setprecision(10) << atom->y << " " << std::setw(16)
                              << std::setprecision(10) << atom->z << std::endl;
#endif
                } else if (rx_done.exactMatch(tempString.c_str())) {
                    myMoleculeList.push_back(molecule);
                    break;
                }
            }
        }
    }
}
