#ifndef __PROGTEST__

#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>

using namespace std;
#endif /* __PROGTEST__ */

class CComponent {
public:
    virtual shared_ptr<CComponent> clone() const = 0;

    virtual void print(ostream &os, const string &padding, bool last = false) const = 0;

};

class CCPU : public CComponent {
private:
    int cores;
    double frequency;

public:
    CCPU(int cores, double frequency) : cores(cores), frequency(frequency) {}

    shared_ptr<CComponent> clone() const {
        return make_shared<CCPU>(*this);
    }

    void print(ostream &os, const string &padding, bool last = false) const {
        os << padding << (last ? "\\-" : "+-") << "CPU, " << cores;
        os << " cores @ " << frequency << "MHz" << endl;
    }
};

class CMemory : public CComponent {
private:
    int megaBytes;
public:
    CMemory(int MiB) : megaBytes(MiB) {}

    shared_ptr<CComponent> clone() const {
        return make_shared<CMemory>(*this);
    }

    void print(ostream &os, const string &padding, bool last = false) const {
        os << padding << (last ? "\\-" : "+-") << "Memory, " << megaBytes << " MiB" << endl;
    }
};

class CDisk : public CComponent {
private:
    bool type;
    int GiB;
    vector<pair<string, int >> partitions;
public:
    static const bool MAGNETIC = true;
    static const bool SSD = false;

    CDisk(bool type, int GiB) : type(type), GiB(GiB) {}

    CDisk &AddPartition(int GiB, const string &id) {
        partitions.emplace_back(id, GiB);
        return *this;
    }

    shared_ptr<CComponent> clone() const {
        return make_shared<CDisk>(*this);
    }

    void print(ostream &os, const string &padding, bool last = false) const {
        os << padding << (last ? "\\-" : "+-") << (type ? "HDD" : "SSD") << ", " << GiB << " GiB" << endl;
        int counter = 0;
        for (auto x : partitions) {
            os << padding << (last ? "  " : "| ") << (x == partitions.back() ? "\\-" : "+-") << "[" << counter
               << "]: " << (x.second) << " GiB, " << x.first << endl;
            counter++;
        }
    }
};

class CComputer {
private:
    string name;
    vector<string> addreses;
    vector<shared_ptr<CComponent>> components;
public:
    CComputer(const string &name) : name(name) {}

    CComputer(const CComputer &pc) : name(pc.name), addreses(pc.addreses) {
        for (auto x : pc.components)
            components.push_back(x->clone());

    }

    CComputer &AddComponent(const CCPU &cpu) {
        components.push_back(make_shared<CCPU>(cpu));
        return *this;
    }

    CComputer &AddComponent(const CMemory &ram) {
        components.push_back(make_shared<CMemory>(ram));
        return *this;
    }

    CComputer &AddComponent(const CDisk &disk) {
        components.push_back(make_shared<CDisk>(disk));
        return *this;
    }

    CComputer &AddAddress(const string &iden) {
        addreses.push_back(iden);
        return *this;
    }

    void print(ostream &os, const string &padding, bool last = false) const {
        os << (last ? "\\-" : "+-") << "Host: " << name << endl;
        for (auto x: addreses) {
            os << padding;
            if (x != addreses.back() || components.size() > 0) {
                os << "+-" << x;
            } else
                os << "\\-" << x;
            os << endl;
        }
        for (auto x: components) {
            if (components.empty()) break;

            if (x != components.back()) {
                x->print(os, padding);
            } else
                x->print(os, padding, true);
        }
    }

    friend bool operator==(const CComputer &a, const CComputer &b) {
        return a.name == b.name;
    }

    friend bool operator!=(const CComputer &a, const CComputer &b) {
        return !(a.name == b.name);
    }

    friend ostream &operator<<(ostream &os, const CComputer &computer) {
        os << "Host: " << computer.name << endl;
        for (auto x: computer.addreses) {
            if (x != computer.addreses.back() || computer.components.size() > 0) {
                os << "+-" << x;
            } else
                os << "\\-" << x;
            os << endl;
        }
        for (auto x: computer.components) {
            if (computer.components.empty()) break;

            if (x != computer.components.back()) {
                x->print(os, "");
            } else
                x->print(os, "", true);
        }
        return os;
    }
};

class CNetwork : public CComponent {
private:
    vector<CComputer> pcs;
    string name;
public:
    CNetwork(const string &name) : name(name) {}

    CComputer * FindComputer(const string &wanted) {
        auto a = std::find(pcs.begin(), pcs.end(), CComputer(wanted));
        return &(pcs[a-pcs.begin()]);
    }

    CNetwork &AddComputer(const CComputer &comp) {
        pcs.push_back(comp);
        return *this;
    }

    void print(ostream &os, const string &padding = "", bool isLast = false) const {
        os << "Network: " << name << endl;
        for (const CComputer & cmp : pcs)
            if (pcs.back() != cmp) {
                cmp.print(os, padding + "| ");
            } else cmp.print(os, padding + "  ", true);
    }

    shared_ptr<CComponent> clone() const {
        return make_shared<CNetwork>(*this);
    }

    friend ostream &operator<<(ostream &os, const CNetwork &cn) {
        cn.print(os, "");
        return os;
    }

};

#ifndef __PROGTEST__

template<typename _T>
string toString(const _T &x) {
    ostringstream oss;
    oss << x;
    return oss.str();
}

int main(void) {
    CNetwork n("FIT network");
    n.AddComputer(
            CComputer("progtest.fit.cvut.cz").
                    AddAddress("147.32.232.142").
                    AddComponent(CCPU(8, 2400)).
                    AddComponent(CCPU(8, 1200)).
                    AddComponent(CDisk(CDisk::MAGNETIC, 1500).
                    AddPartition(50, "/").
                    AddPartition(5, "/boot").
                    AddPartition(1000, "/var")).
                    AddComponent(CDisk(CDisk::SSD, 60).
                    AddPartition(60, "/data")).
                    AddComponent(CMemory(2000)).
                    AddComponent(CMemory(2000))).
            AddComputer(
            CComputer("edux.fit.cvut.cz").
                    AddAddress("147.32.232.158").
                    AddComponent(CCPU(4, 1600)).
                    AddComponent(CMemory(4000)).
                    AddComponent(CDisk(CDisk::MAGNETIC, 2000).
                    AddPartition(100, "/").
                    AddPartition(1900, "/data"))).
            AddComputer(
            CComputer("imap.fit.cvut.cz").
                    AddAddress("147.32.232.238").
                    AddComponent(CCPU(4, 2500)).
                    AddAddress("2001:718:2:2901::238").
                    AddComponent(CMemory(8000)));
    string a;
    assert (toString(n) ==
            "Network: FIT network\n"
            "+-Host: progtest.fit.cvut.cz\n"
            "| +-147.32.232.142\n"
            "| +-CPU, 8 cores @ 2400MHz\n"
            "| +-CPU, 8 cores @ 1200MHz\n"
            "| +-HDD, 1500 GiB\n"
            "| | +-[0]: 50 GiB, /\n"
            "| | +-[1]: 5 GiB, /boot\n"
            "| | \\-[2]: 1000 GiB, /var\n"
            "| +-SSD, 60 GiB\n"
            "| | \\-[0]: 60 GiB, /data\n"
            "| +-Memory, 2000 MiB\n"
            "| \\-Memory, 2000 MiB\n"
            "+-Host: edux.fit.cvut.cz\n"
            "| +-147.32.232.158\n"
            "| +-CPU, 4 cores @ 1600MHz\n"
            "| +-Memory, 4000 MiB\n"
            "| \\-HDD, 2000 GiB\n"
            "|   +-[0]: 100 GiB, /\n"
            "|   \\-[1]: 1900 GiB, /data\n"
            "\\-Host: imap.fit.cvut.cz\n"
            "  +-147.32.232.238\n"
            "  +-2001:718:2:2901::238\n"
            "  +-CPU, 4 cores @ 2500MHz\n"
            "  \\-Memory, 8000 MiB\n");

    CNetwork x = n;
    auto c = x.FindComputer("imap.fit.cvut.cz");
    assert (toString(*c) ==
            "Host: imap.fit.cvut.cz\n"
            "+-147.32.232.238\n"
            "+-2001:718:2:2901::238\n"
            "+-CPU, 4 cores @ 2500MHz\n"
            "\\-Memory, 8000 MiB\n");
    c->AddComponent(CDisk(CDisk::MAGNETIC, 1000).
            AddPartition(100, "system").
            AddPartition(200, "WWW").
            AddPartition(700, "mail"));
    assert (toString(x) ==
            "Network: FIT network\n"
            "+-Host: progtest.fit.cvut.cz\n"
            "| +-147.32.232.142\n"
            "| +-CPU, 8 cores @ 2400MHz\n"
            "| +-CPU, 8 cores @ 1200MHz\n"
            "| +-HDD, 1500 GiB\n"
            "| | +-[0]: 50 GiB, /\n"
            "| | +-[1]: 5 GiB, /boot\n"
            "| | \\-[2]: 1000 GiB, /var\n"
            "| +-SSD, 60 GiB\n"
            "| | \\-[0]: 60 GiB, /data\n"
            "| +-Memory, 2000 MiB\n"
            "| \\-Memory, 2000 MiB\n"
            "+-Host: edux.fit.cvut.cz\n"
            "| +-147.32.232.158\n"
            "| +-CPU, 4 cores @ 1600MHz\n"
            "| +-Memory, 4000 MiB\n"
            "| \\-HDD, 2000 GiB\n"
            "|   +-[0]: 100 GiB, /\n"
            "|   \\-[1]: 1900 GiB, /data\n"
            "\\-Host: imap.fit.cvut.cz\n"
            "  +-147.32.232.238\n"
            "  +-2001:718:2:2901::238\n"
            "  +-CPU, 4 cores @ 2500MHz\n"
            "  +-Memory, 8000 MiB\n"
            "  \\-HDD, 1000 GiB\n"
            "    +-[0]: 100 GiB, system\n"
            "    +-[1]: 200 GiB, WWW\n"
            "    \\-[2]: 700 GiB, mail\n");
    cout << toString(n) << endl;
    a = "Network: FIT network\n"
        "+-Host: progtest.fit.cvut.cz\n"
        "| +-147.32.232.142\n"
        "| +-CPU, 8 cores @ 2400MHz\n"
        "| +-CPU, 8 cores @ 1200MHz\n"
        "| +-HDD, 1500 GiB\n"
        "| | +-[0]: 50 GiB, /\n"
        "| | +-[1]: 5 GiB, /boot\n"
        "| | \\-[2]: 1000 GiB, /var\n"
        "| +-SSD, 60 GiB\n"
        "| | \\-[0]: 60 GiB, /data\n"
        "| +-Memory, 2000 MiB\n"
        "| \\-Memory, 2000 MiB\n"
        "+-Host: edux.fit.cvut.cz\n"
        "| +-147.32.232.158\n"
        "| +-CPU, 4 cores @ 1600MHz\n"
        "| +-Memory, 4000 MiB\n"
        "| \\-HDD, 2000 GiB\n"
        "|   +-[0]: 100 GiB, /\n"
        "|   \\-[1]: 1900 GiB, /data\n"
        "\\-Host: imap.fit.cvut.cz\n"
        "  +-147.32.232.238\n"
        "  +-2001:718:2:2901::238\n"
        "  +-CPU, 4 cores @ 2500MHz\n"
        "  \\-Memory, 8000 MiB\n";
    cout << a << endl;
    assert (toString(n) ==
            "Network: FIT network\n"
            "+-Host: progtest.fit.cvut.cz\n"
            "| +-147.32.232.142\n"
            "| +-CPU, 8 cores @ 2400MHz\n"
            "| +-CPU, 8 cores @ 1200MHz\n"
            "| +-HDD, 1500 GiB\n"
            "| | +-[0]: 50 GiB, /\n"
            "| | +-[1]: 5 GiB, /boot\n"
            "| | \\-[2]: 1000 GiB, /var\n"
            "| +-SSD, 60 GiB\n"
            "| | \\-[0]: 60 GiB, /data\n"
            "| +-Memory, 2000 MiB\n"
            "| \\-Memory, 2000 MiB\n"
            "+-Host: edux.fit.cvut.cz\n"
            "| +-147.32.232.158\n"
            "| +-CPU, 4 cores @ 1600MHz\n"
            "| +-Memory, 4000 MiB\n"
            "| \\-HDD, 2000 GiB\n"
            "|   +-[0]: 100 GiB, /\n"
            "|   \\-[1]: 1900 GiB, /data\n"
            "\\-Host: imap.fit.cvut.cz\n"
            "  +-147.32.232.238\n"
            "  +-2001:718:2:2901::238\n"
            "  +-CPU, 4 cores @ 2500MHz\n"
            "  \\-Memory, 8000 MiB\n");

    return 0;
}

#endif /* __PROGTEST__ */
