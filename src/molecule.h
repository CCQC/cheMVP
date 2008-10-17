#ifndef MOLECULE_H_
#define MOLECULE_H_

#include <QString>
#include <vector>

struct AtomEntry{
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
	
	void addAtom(AtomEntry *atom) {myMolecule.push_back(atom);}
	void setComment(QString comment) {myComment = comment;}
	std::vector<AtomEntry*>& atomsList() {return myMolecule;}

    int numAtoms() const { return myMolecule.size(); }
private:
	std::vector<AtomEntry*> myMolecule;
	QString myComment;
};
#endif /*MOLECULE_H_*/
