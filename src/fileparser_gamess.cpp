#include <ios>
#include "fileparser.h"
using namespace std;

void FileParser::readGamess()
{    
    std::string tempString;    
    QRegExp rx("", Qt::CaseInsensitive, QRegExp::RegExp2);
    
    while (1) {
        Molecule *molecule = new Molecule();
        
        // GOAL TO MATCH -
        // COORDINATES OF ALL ATOMS ARE (ANGS)
        //   ATOM   CHARGE       X              Y              Z
        // ------------------------------------------------------------
        // H           1.0  -6.2725378445  -1.0334635451   0.0079340274
        // N           7.0  -0.9574887943   0.4321902177  -0.1657223307
        // C           6.0  -3.5958441565   0.8065453305  -0.0645835769
        // H           1.0  -5.6307773124   1.4561856910   0.1992947621
        // N           7.0  -1.1350045749  -1.8957671498  -0.1866507391
        // C           6.0   3.9800481487  -0.9332491491   0.0742753853
        // C           6.0   4.6322964081   0.2562374937   0.1067233629
        // N           7.0  -4.1756009798  -1.4243169927  -0.0935543004
        // C           6.0  -5.2527867336  -0.6653878599   0.0303849944
        // H           1.0   5.7174335753   0.3249034724   0.1583938690
        // N           7.0  -2.8578425881   1.9317029691   0.0421121541
        // H           1.0   4.4694529569   2.3426813331   0.1405137782
        // H           1.0  -0.8638559178   2.4917407077  -0.0994618657
        // H           1.0  -1.7182005834  -2.7229288811  -0.0725441257
        // C           6.0  -1.7260135963  -0.6846508891  -0.0935283080
        // N           7.0  -4.9705527096   0.6869777112   0.1103143230
        // H           1.0   4.3836834222  -2.8768275496  -0.7556104548
        // C           6.0  -3.1306433824  -0.5116178606  -0.1192841800
        // H           1.0   0.8791477942   0.3929183584  -0.0385674506
        // H           1.0   4.3978306507  -2.8311897474   1.0049921129
        // H           1.0   5.7640781051  -2.1393632216   0.0922828812
        // C           6.0   4.6752299710  -2.2643899912   0.1071363048
        // C           6.0  -1.5519373304   1.6468436174   0.0212975256
        // H           1.0  -0.1146451992  -1.9781595955  -0.0313572330
        // N           7.0   3.9646593022   1.4636970792   0.0755065622
        // C           6.0   2.5760268809   1.5787741503   0.0052292719
        // O           8.0   2.0054709373   2.6628659806   0.0221755671
        // N           7.0   1.9281844086   0.3566034101   0.0281244464
        // C           6.0   2.5153456969  -0.9065456189   0.0945565541
        // O           8.0   1.8165225237  -1.9298676636   0.1283661943
        
        getline(infile, tempString);
        while (tempString.find("COORDINATES OF ALL ATOMS ARE") == string::npos && infile.eof() == false) {
            getline(infile, tempString);
        }
        if (infile.eof()) 
            break;
        #ifdef QT_DEBUG        
        std::cout << "readGamess: 'COORDINATES OF ALL ATOMS ARE' found.\n";
        #endif
        
        getline(infile, tempString);
        getline(infile, tempString);
        
        // qchem31 is reported in Angstroms
        myUnits = Angstrom;
        
        // Read in atom information
        rx.setPattern("(?:\\s*)(\\w+)(?:\\s+)(?:\\d+.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)(?:\\s+)(-?\\d+\\.\\d+)");
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
