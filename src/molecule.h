#ifndef MOLECULE_H_
#define MOLECULE_H_

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <vector>

struct AtomEntry {
    QString Label;
    double x;
    double y;
    double z;
};

class Molecule
{
  public:
    Molecule()
    {
    }
    ~Molecule()
    {
    }

    void addAtom(AtomEntry *atom)
    {
        _molecule.push_back(atom);
    }
    void setComment(QString c)
    {
        _comment = c;
    }
    std::vector<AtomEntry *> &atomsList()
    {
        return _molecule;
    }
    int numAtoms() const
    {
        return _molecule.size();
    }

    void serialize(QXmlStreamWriter *writer)
    {
        writer->writeStartElement("Molecule");
        writer->writeAttribute("items", QString("%1").arg(_molecule.size()));
        writer->writeAttribute("comment", _comment);
        foreach (AtomEntry *a, _molecule) {
            writer->writeStartElement("AtomEntry");
            writer->writeAttribute("label", a->Label);
            writer->writeAttribute("x", QString("%1").arg(a->x));
            writer->writeAttribute("y", QString("%1").arg(a->y));
            writer->writeAttribute("z", QString("%1").arg(a->z));
            writer->writeEndElement();
        }
        writer->writeEndElement();
    };

    static Molecule *deserialize(QXmlStreamReader *reader)
    {
        reader->readNextStartElement();
        Q_ASSERT(reader->isStartElement() && reader->name() == "Molecule");

        Molecule *m = new Molecule();
        int size = reader->attributes().value("items").toString().toInt();
        for (int i = 0; i < size; i++) {
            reader->readNextStartElement();
            Q_ASSERT(reader->isStartElement() && reader->name().toString() == "AtomEntry");

            AtomEntry *a = new AtomEntry();
            a->Label = reader->attributes().value("label").toString();
            a->x = reader->attributes().value("x").toString().toDouble();
            a->y = reader->attributes().value("y").toString().toDouble();
            a->z = reader->attributes().value("z").toString().toDouble();
            m->addAtom(a);
            reader->skipCurrentElement();
        }
        reader->skipCurrentElement();
        return m;
    };

  private:
    std::vector<AtomEntry *> _molecule;
    QString _comment;
};

#endif /*MOLECULE_H_*/
