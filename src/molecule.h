#ifndef MOLECULE_H_
#define MOLECULE_H_

#include <QString>
#include <vector>

struct AtomEntry
{
    QString Label;
    double x;
    double y;
    double z;
};

class Molecule
{
public:
    Molecule();
    ~Molecule();

    void addAtom(AtomEntry *atom) {_molecule.push_back(atom);}
    void setComment(QString c) {_comment = c;}
    std::vector<AtomEntry*>& atomsList() {return _molecule;}
    int numAtoms() const {return _molecule.size();}

private:
    std::vector<AtomEntry*> _molecule;
    QString _comment;
};

#endif /*MOLECULE_H_*/
