#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

using namespace std;

class InvalidRangeException {
};

#endif /* __PROGTEST__ */
 #define EXTENDED_SYNTAX
class CRangeList;

class CRange {
public:
    CRange() {}

    CRange(long long num) {
        lower = higher = num;
    }

    CRange(long long lo, long long hi) {
        if (hi < lo) throw InvalidRangeException();
        lower = lo;
        higher = hi;
    }

    // check if number is distant by one unit
    bool hasRightNeighbour(long long number) const {
        if (higher == number) return true;
        return higher > number ? number + 1 == higher : higher + 1 == number;
    }
    bool hasLeftNeighbour(long long number) const {
        if (lower == number) return true;
        return lower < number ? lower + 1 == number : number + 1 == lower;
    }

    // check if interval or number is within a range
    bool includes(long long num) const {
        return lower <= num && num <= higher;
    }

    // sets new value
    bool setHigherUpper(long long num) {
        if (num > higher)
            higher = num;
        return true;
    }
    bool setLesserLower(long long num) {
        if (num < lower)
            lower = num;
        return true;
    }
    bool setNewBounds(long long newLow, long long newHigh) {
        return setHigherUpper(newHigh) && setLesserLower(newLow);
    }

    friend class CRangeList;
    friend ostream &operator<<(ostream &os, CRange const &cr);

    // comparators
    friend bool lowerInRange(const CRange &left, const CRange &right);
    friend bool higherInRange(const CRange &left, const CRange &right);

    bool operator==(const CRange &ref) const;
    bool operator!=(const CRange &ref) const;
    bool operator<(const CRange &ref) const;
    bool operator>(const CRange &ref) const;

    CRangeList operator+(const CRange & ref) const;
    CRangeList operator-(const CRange & ref) const;

private:

    long long lower, higher;
};

ostream &operator<<(ostream &os, CRange const &cr)  {
    ostringstream ostring;

    if(cr.lower == cr.higher){
        ostring<<cr.higher;
        os<<ostring.str();
        return os;
    }

    ostring << "<" << cr.lower << ".." << cr.higher<< ">";

    os<<ostring.str();
    return os;
}

// comparators
bool lowerInRange(const CRange &left, const CRange &right) {
    return left.lower < right.lower;
}
bool higherInRange(const CRange &left, const CRange &right) {
    return left.higher < right.higher;
}

bool CRange::operator==(const CRange &ref) const {
    return this->lower == ref.lower && this->higher == ref.higher;
}
bool CRange::operator!=(const CRange &ref) const {
    return !(*this==ref);
}
bool CRange::operator>(const CRange &ref) const {
    return this->lower > ref.higher && !ref.hasRightNeighbour(this->lower);
}
bool CRange::operator<(const CRange &ref) const {
    return ref > *this;
}

class CRangeList {
public:
    // constructors
    CRangeList() = default;
    CRangeList(std::initializer_list<CRange> val){
        for(auto x : val)
            Add(x);
    }

    //operators
    friend ostream &operator<<(ostream &os, CRangeList const &crl);

    CRangeList& operator=(const CRange & ran);
    bool operator==(const CRangeList &ref ) const;
    bool operator!=(const CRangeList &ref ) const;
    CRangeList& operator+=(const CRange& ref);
    CRangeList& operator-=(const CRange & ref);
    CRangeList& operator+=(const CRangeList& ref);
    CRangeList& operator-=(const CRangeList& ref);
    CRangeList operator-(const CRange& ref) const;
    CRangeList operator-(const CRangeList& ref) const;
    CRangeList operator+(const CRange& ref) const;
    CRangeList operator+(const CRangeList& ref) const;

    vector<CRange>::const_iterator begin() const {
        return rangeVec.begin();
    }
    vector<CRange>::const_iterator end() const {
        return rangeVec.end();
    }

    // data holder
    vector<CRange> rangeVec;

    void Add(const CRange &cr);
    void Del(const CRange &cr);
    bool Includes(long long number) const;
    bool Includes(const CRange & ref) const;
};

CRangeList CRange::operator+(const CRange &ref) const {
    CRangeList c;
    c.Add(*this);
    c.Add(ref);
    return c;
}

CRangeList CRange::operator-(const CRange &ref) const {
    CRangeList c;
    c.Add(*this);
    c.Del(ref);
    return c;
}

void CRangeList::Add(const CRange &cr) {
    // if empty -> pushback
    if (rangeVec.empty()) {
        rangeVec.push_back(cr);
        return;
    }
    // if new one includes all existing -> clear and add
    if ((*rangeVec.begin()).lower >= cr.higher && rangeVec.back().higher <= cr.lower) {
        rangeVec.clear();
        rangeVec.push_back(cr);
        return;
    }
    auto lower = lower_bound(rangeVec.begin(), rangeVec.end(), cr, lowerInRange);
    if (lower == rangeVec.end()) {
        // check if last is left neighbour
        if (rangeVec.back().hasRightNeighbour(cr.lower) || rangeVec.back().includes(cr.lower)) {
            rangeVec.back().setHigherUpper(cr.higher);
            return;
        } else rangeVec.push_back(cr);
        return;
    }
    if (lower == rangeVec.begin()) { //
        if (*lower > cr) {
            rangeVec.insert(lower, cr);
            return;
        }
        if ((*lower).includes(cr.higher) || (*lower).hasLeftNeighbour(cr.higher)) {
            (*lower).setLesserLower(cr.lower);
            return;
        }
    }
    auto higher = lower_bound(rangeVec.begin(), rangeVec.end(), cr, higherInRange);
    // check if iter does not before contains lower
    if (!(*lower).includes(cr.lower) && lower != rangeVec.begin()) {
        // check if the one before contains value or is neighbour
        if ((*(lower - 1)).includes(cr.lower) || (*(lower - 1)).hasRightNeighbour(cr.lower))
            lower--;
    }
    //
    if (higher == rangeVec.end()) {
        long long min = (*lower).lower < cr.lower ? (*lower).lower : cr.lower;
        rangeVec.erase(lower, higher);
        rangeVec.emplace_back(min, cr.higher);
        return;
    }

    if (!(*higher).includes(cr.higher) && !(*higher).hasLeftNeighbour(cr.higher)) {
        /*  if((*(higher-1)).hasRightNeighbour(cr.higher))*/
        higher--;
    }
    CRange a{cr.lower, cr.higher};
    a.setNewBounds((*lower).lower, (*higher).higher);
    rangeVec.erase(lower, higher + 1);
    auto newPos = lower_bound(rangeVec.begin(), rangeVec.end(), a, lowerInRange);
    rangeVec.insert(newPos, a);
}
void CRangeList::Del(const CRange &cr) {
    if (rangeVec.empty())
        return;

    if ((*rangeVec.begin()).lower >= cr.higher && rangeVec.back().higher <= cr.lower) {
        rangeVec.clear();
        return;
    }

    auto lower = lower_bound(rangeVec.begin(), rangeVec.end(), cr, lowerInRange);

    // dostal jsem se za vector
    if (lower == rangeVec.end()) {
        // posledni obsahuje nizsi referencni
        if (rangeVec.back().includes(cr.lower)) {
            // referencni prekryva posledni high
            if (rangeVec.back().higher <= cr.higher) {
                rangeVec.back().higher = cr.lower - 1;
                return;
            }
            CRange tmp(cr.higher + 1, rangeVec.back().higher);
            rangeVec.back().higher = cr.lower - 1;
            rangeVec.push_back(tmp);
            return;
        }
        // je vetsi nez maximalni
        return;
    }
    // jsem na prvnim prvku
    if (lower == rangeVec.begin()) {
        // je mensi nez minimalni
        if (cr.higher < lower->lower) {
            return;
        }
        if (lower->lower < cr.lower && cr.higher < lower->higher) {
            long min = lower->lower;
            lower->lower = cr.higher + 1;
            rangeVec.insert(lower, {min, cr.lower - 1});
            return;
        }
        // prvek obsahuje referencni vetsi
        if (lower->includes(cr.higher)) {
            if (cr.lower <= lower->lower && cr.higher == lower->higher) {
                rangeVec.erase(lower);
                return;
            }
            if (cr.lower <= lower->lower) {
                lower->lower = cr.higher + 1;
                return;
            }
        }
    }

    auto higher = lower_bound(rangeVec.begin(), rangeVec.end(), cr, higherInRange);
    // aktualni leava neobsahuje mensi odebiranou
    if(!lower->includes(cr.lower) && lower != rangeVec.begin()){
        // predchozi ano - > zmensim iterator
        if((lower-1)->includes(cr.lower)){
            lower--;
        }
    }
    // prava je na konci a mam prvni zacatek intervalu  vetsi rovnej referencnimu
    if(higher == rangeVec.end()){
        long min = lower->lower;
        rangeVec.erase(lower, higher);
        if(min < cr.lower)
            rangeVec.emplace_back(min, cr.lower-1);
        return;
    }
    if(!higher->includes(cr.higher) && higher != rangeVec.begin()){
        if((higher-1)->includes(cr.higher))
            higher--;
    }
    if(cr.higher<lower->lower){
        //  cout<<"Neni prunik"<<endl;
        return;
    }
    long min = lower->lower;
    long max = higher->higher;
    long lmax = higher->lower;
    rangeVec.erase(lower, higher+1);
    if(min<cr.lower){
        CRange a(min, cr.lower-1);
        auto newPos = lower_bound(rangeVec.begin(), rangeVec.end(), a, lowerInRange);
        rangeVec.insert(newPos, a);
    }
    if(max > cr.higher){
        CRange b{lmax, max};
        if(b.includes(cr.higher))
            b.lower = (cr.higher+1);
        auto newPos = lower_bound(rangeVec.begin(), rangeVec.end(), b, lowerInRange);
        rangeVec.insert(newPos, b);
    }

    //cout << (*lower) << " : " << (*higher) << endl;
}

// Overloaded operators
CRangeList &CRangeList::operator=(const CRange &ran) {
    this->rangeVec.clear();
    Add(ran);
    return *this;
}
ostream &operator<<(ostream &os, CRangeList const &crl) {
    ostringstream ostring;

    ostring << "{";
    for (auto x : crl.rangeVec) {
        ostring << x;
        if (x != crl.rangeVec.back())
            ostring << ",";
    }
    ostring << "}";
    os<<ostring.str();

    return os;
}

bool CRangeList::operator==(const CRangeList &ref) const {
    if(rangeVec.size() != ref.rangeVec.size()){
        return false;
    }
    for(size_t idx = 0; idx<rangeVec.size(); idx++)
        if(rangeVec[idx] != ref.rangeVec[idx])
            return false;
    return true;
}
bool CRangeList::operator!=(const CRangeList &ref) const {
    return !(*this == ref);
}

CRangeList &CRangeList::operator+=(const CRange &ref) {
    Add(ref);
    return *this;
}

CRangeList &CRangeList::operator-=(const CRange &ref) {
    Del(ref);
    return *this;
}

CRangeList &CRangeList::operator+=(const CRangeList &ref) {
    for(auto x : ref.rangeVec)
        Add(x);
    return *this;
}

CRangeList &CRangeList::operator-=(const CRangeList &ref) {
    for(auto x: ref.rangeVec)
        Del(x);
    return *this;
}

CRangeList CRangeList::operator-(const CRange &ref) const {
    CRangeList c = *this;
    c.Del(ref);
    return c;
}

CRangeList CRangeList::operator-(const CRangeList &ref) const {
    CRangeList c = *this;
    for (auto x: ref.rangeVec)
        c.Del(x);
    return c;
}

CRangeList CRangeList::operator+(const CRange &ref) const {
    CRangeList c = *this;
    c.Add(ref);
    return c;
}

CRangeList CRangeList::operator+(const CRangeList &ref) const {
    CRangeList c = *this;
    for (auto x: ref.rangeVec)
        c.Add(x);
    return c;
}

bool CRangeList::Includes(long long number) const {
    if( rangeVec.empty())
        return false;

    CRange a(number);
    auto numIter = lower_bound(rangeVec.begin(), rangeVec.end(), a, lowerInRange);
    if(numIter == rangeVec.end()){
        return rangeVec.back().includes(number);
    }
    if(numIter->includes(number))
        return true;
    if(numIter != rangeVec.begin()){
        return (numIter-1)->includes(number);
    }
    else return false;

}

bool CRangeList::Includes(const CRange &ref) const {
    if(rangeVec.empty()){
        return false;
    }
    auto rangeIter = lower_bound(rangeVec.begin(), rangeVec.end(), ref, lowerInRange);
    if(rangeIter == rangeVec.end()){
        return (rangeIter-1)->includes(ref.lower) && (rangeIter-1)->includes(ref.higher);
    }
    if((rangeIter)->includes(ref.lower) && (rangeIter)->includes(ref.higher))
        return true;

    if( rangeIter != rangeVec.begin() && (rangeIter-1)->includes(ref.lower) && (rangeIter-1)->includes(ref.higher))
        return true;
    return false;
}


#ifndef __PROGTEST__

string toString(const CRangeList &x) {
    ostringstream oss;
    oss << x;
    return oss.str();
}

int main(void) {



    CRangeList a, b;

    assert ( sizeof ( CRange ) <= 2 * sizeof ( long long ) );
    a = CRange ( 5, 10 );
    a += CRange ( 25, 100 );
    assert ( toString ( a ) == "{<5..10>,<25..100>}" );
    a += CRange ( -5, 0 );
    a += CRange ( 8, 50 );
    assert ( toString ( a ) == "{<-5..0>,<5..100>}" );
    a += CRange ( 101, 105 ) + CRange ( 120, 150 ) + CRange ( 160, 180 ) + CRange ( 190, 210 );
    assert ( toString ( a ) == "{<-5..0>,<5..105>,<120..150>,<160..180>,<190..210>}" );
    a += CRange ( 106, 119 ) + CRange ( 152, 158 );
    assert ( toString ( a ) == "{<-5..0>,<5..150>,<152..158>,<160..180>,<190..210>}" );
    a += CRange ( -3, 170 );
    a += CRange ( -30, 1000 );
    assert ( toString ( a ) == "{<-30..1000>}" );
    b = CRange ( -500, -300 ) + CRange ( 2000, 3000 ) + CRange ( 700, 1001 );
    a += b;
    assert ( toString ( a ) == "{<-500..-300>,<-30..1001>,<2000..3000>}" );
    a -= CRange ( -400, -400 );
    assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..1001>,<2000..3000>}" );
    a -= CRange ( 10, 20 ) + CRange ( 900, 2500 ) + CRange ( 30, 40 ) + CRange ( 10000, 20000 );
    assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
    try
    {
        a += CRange ( 15, 18 ) + CRange ( 10, 0 ) + CRange ( 35, 38 );
        assert ( "Exception not thrown" == NULL );
    }
    catch ( const InvalidRangeException & e )
    {
    }
    catch ( ... )
    {
        assert ( "Invalid exception thrown" == NULL );
    }
    assert ( toString ( a ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
    b = a;
    assert ( a == b );
    assert ( !( a != b ) );
    b += CRange ( 2600, 2700 );
    assert ( toString ( b ) == "{<-500..-401>,<-399..-300>,<-30..9>,<21..29>,<41..899>,<2501..3000>}" );
    assert ( a == b );
    assert ( !( a != b ) );
    b += CRange ( 15, 15 );
    assert ( toString ( b ) == "{<-500..-401>,<-399..-300>,<-30..9>,15,<21..29>,<41..899>,<2501..3000>}" );
    assert ( !( a == b ) );
    assert ( a != b );
    assert ( b . Includes ( 15 ) );
    assert ( b . Includes ( 2900 ) );
    assert ( b . Includes ( CRange ( 15, 15 ) ) );
    assert ( b . Includes ( CRange ( -350, -350 ) ) );
    assert ( b . Includes ( CRange ( 100, 200 ) ) );
    assert ( !b . Includes ( CRange ( 800, 900 ) ) );
    assert ( !b . Includes ( CRange ( -1000, -450 ) ) );
    assert ( !b . Includes ( CRange ( 0, 500 ) ) );
    a += CRange ( -10000, 10000 ) + CRange ( 10000000, 1000000000 );
    assert ( toString ( a ) == "{<-10000..10000>,<10000000..1000000000>}" );
    b += a;
    assert ( toString ( b ) == "{<-10000..10000>,<10000000..1000000000>}" );
    b -= a;
    assert ( toString ( b ) == "{}" );
    b += CRange ( 0, 100 ) + CRange ( 200, 300 ) - CRange ( 150, 250 ) + CRange ( 160, 180 ) - CRange ( 170, 170 );
    assert ( toString ( b ) == "{<0..100>,<160..169>,<171..180>,<251..300>}" );
    b -= CRange ( 10, 90 ) - CRange ( 20, 30 ) - CRange ( 40, 50 ) - CRange ( 60, 90 ) + CRange ( 70, 80 );
    assert ( toString ( b ) == "{<0..9>,<20..30>,<40..50>,<60..69>,<81..100>,<160..169>,<171..180>,<251..300>}" );
#ifdef EXTENDED_SYNTAX
    CRangeList x { { 5, 20 }, { 150, 200 }, { -9, 12 }, { 48, 93 } };
  assert ( toString ( x ) == "{<-9..20>,<48..93>,<150..200>}" );
  ostringstream oss;
  oss << setfill ( '=' ) << hex << left;
  for ( const auto & v : x + CRange ( -100, -100 ) )
    oss << v << endl;
  oss << setw ( 10 ) << 1024;
    assert ( oss . str () == "-100\n<-9..20>\n<48..93>\n<150..200>\n400=======" );
#endif /* EXTENDED_SYNTAX */

    return 0;

}

#endif /* __PROGTEST__ */
