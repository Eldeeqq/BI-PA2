#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

using namespace std;
#endif /* __PROGTEST__ */
struct Land{
    static unsigned counter;
    string  city, address, region, owner = "", owner_lower = "";
    unsigned id, uid;
    Land(const string & city, const string & address) : city(city), address(address){}
    Land(const string & region, const unsigned & id) : region(region), id(id){}
    Land(const string& city, const string& addr, const string& region, unsigned int id, const string & owner="" )
    : city(city), address(addr), region(region), id(id) {
        uid = ++counter;
        this->owner = owner;
        this->owner_lower = owner;
        transform(owner_lower.begin(), owner_lower.end(), owner_lower.begin(), ::tolower);
    }
    bool setNewOwner(const string & owner){
        this->owner = owner;
        this->owner_lower = owner;
        transform(owner_lower.begin(), owner_lower.end(), owner_lower.begin(), ::tolower);
        return true;
    }
    bool newUID(){
        uid = ++counter;
        return true;
    }
};
unsigned Land::counter = 0;


static bool compareAddress(Land * const & l, Land * const & r ){
    if(l->city == r->city)
        return l->address < r->address;
    return l->city < r->city;
}

static bool compareRegion(Land * const & l, Land * const & r ){
    if(l->region == r->region)
        return l->id < r->id;
    return l->region < r->region;
}

static bool compareUID(Land * const & l, Land * const & r){
    return l->uid < r->uid;
}

struct Owner{
    string name;
    vector<Land *> owns;
    Owner(){}
    Owner(const string & name){
        this->name = name;
        transform(this->name.begin(), this->name.end(), this->name.begin(), ::tolower);
    }
};

static bool compareOwner(const Owner & l, const Owner  & r){
    return l.name < r.name;
}


class CIterator {
public:
    bool AtEnd(void) const;

    void Next(void);

    string City(void) const;

    string Addr(void) const;

    string Region(void) const;

    unsigned ID(void) const;

    string Owner(void) const;

    CIterator(vector<Land*>::const_iterator begin, vector<Land*>::const_iterator end) : begin(begin), end(end){

    }
private:
    vector<Land*>::const_iterator begin;
    vector<Land*>::const_iterator end;
};

bool CIterator::AtEnd(void) const {
    return begin == end;
}

string CIterator::City(void) const {
    return (*begin)->city;
}

string CIterator::Addr(void) const {
    return (*begin)->address;
}

string CIterator::Region(void) const {
    return (*begin)->region;
}

unsigned CIterator::ID(void) const {
    return (*begin)->id;
}

string CIterator::Owner(void) const {
    return (*begin)->owner;
}

void CIterator::Next(void) {
    begin++;

}

class CLandRegister {
public:
    bool Add(const string &city,
             const string &addr,
             const string &region,
             unsigned int id);

    bool Del(const string &city,
             const string &addr);

    bool Del(const string &region,
             unsigned int id);

    bool GetOwner(const string &city,
                  const string &addr,
                  string &owner) const;

    bool GetOwner(const string &region,
                  unsigned int id,
                  string &owner) const;

    bool NewOwner(const string &city,
                  const string &addr,
                  const string &owner);

    bool NewOwner(const string &region,
                  unsigned int id,
                  const string &owner);

    unsigned Count(const string &owner) const;

   CIterator ListByAddr(void) const {
       return CIterator(sortedByAddress.begin(), sortedByAddress.end());
   }

    CIterator ListByOwner(const string &owner) const{
       Owner temp = {owner};
       auto ownIter = lower_bound(sortedByOwner.begin(), sortedByOwner.end(), temp, compareOwner);
       if(indexOfOwner(owner, ownIter)){
           return CIterator(ownIter->owns.begin(), ownIter->owns.end());
       }
        return CIterator(sortedByAddress.end(), sortedByAddress.end());
   }
    ~CLandRegister(){
       for (auto i : sortedByAddress)
           delete i;
   }
private:
  vector<Land*> sortedByAddress;
  vector<Land*> sortedByRegion;
  vector<Owner> sortedByOwner;

  bool existsInAddr(const string &city, const string &addr) const{
      Land * tmp = new Land(city, addr);
      auto lookup = lower_bound(sortedByAddress.begin(), sortedByAddress.end(), tmp, compareAddress);
      delete tmp;
      if( lookup != sortedByAddress.end()){
          if((*lookup)->city == city && (*lookup)->address == addr)
              return true;
      }
      return false;
  }

  bool existsInRegion(const string &region, unsigned int id) const {
     Land * tmp = new Land(region, id);
     auto lookup = lower_bound(sortedByRegion.begin(), sortedByRegion.end(), tmp, compareRegion);
     delete tmp;
     if( lookup != sortedByRegion.end()){
         if((*lookup)->region == region && (*lookup)->id == id)
             return true;
     }
     return false;
  }

  bool indexOfOwner(const string & owner, const vector<Owner>::const_iterator & idx) const {
      Owner tmp(owner);
      if(idx != sortedByOwner.end()){
          if(tmp.name == (*idx).name){
              return true;
          }
      }
      return false;
  }
};

bool CLandRegister::Add(const string &city, const string &addr, const string &region, unsigned int id) {
    if(existsInAddr(city, addr) || existsInRegion(region, id)) // check if combinations already exist
        return false;

    Land * newLand = new Land(city, addr, region, id);

    // find indexes where to insert
    auto addrIterator = lower_bound(sortedByAddress.begin(), sortedByAddress.end(), newLand, compareAddress);
    auto regIterator = lower_bound(sortedByRegion.begin(), sortedByRegion.end(), newLand, compareRegion);
    Owner tmp = {""};
    auto ownerIndex = lower_bound(sortedByOwner.begin(), sortedByOwner.end(), tmp, compareOwner);

    if(!indexOfOwner("", ownerIndex)){
        Owner defOwner("");
        ownerIndex = sortedByOwner.insert(ownerIndex, defOwner);
    }

    // insert to vector
    sortedByAddress.insert(addrIterator, newLand);
    sortedByRegion.insert(regIterator, newLand);

    ownerIndex->owns.push_back(newLand);
    return true;
}

bool CLandRegister::Del(const string &city, const string &addr) {
   if(!existsInAddr(city, addr))
       return false;

    Land * tmp = new Land(city, addr);
    auto addrIterator = lower_bound(sortedByAddress.begin(), sortedByAddress.end(), tmp, compareAddress);
    tmp->region = (*addrIterator)->region;
    tmp->id = (*addrIterator)->id;
    auto regIterator = lower_bound(sortedByRegion.begin(), sortedByRegion.end(), tmp, compareRegion);

    Owner tempOwner = {(*addrIterator)->owner_lower};
    auto ownerIndex = lower_bound(sortedByOwner.begin(), sortedByOwner.end(), tempOwner, compareOwner);
    tmp->uid = (*addrIterator)->uid;
    auto inOwnerIter = lower_bound(ownerIndex->owns.begin(), ownerIndex->owns.end(), tmp, compareUID);

    delete tmp;
    delete (*addrIterator);
    ownerIndex->owns.erase(inOwnerIter);
    sortedByAddress.erase(addrIterator);
    sortedByRegion.erase(regIterator);

   return true;
}

bool CLandRegister::Del(const string &region, unsigned int id) {
    if(!existsInRegion(region, id))
        return false;

    Land * tmp = new Land(region, id);
    auto regIterator = lower_bound(sortedByRegion.begin(), sortedByRegion.end(), tmp, compareRegion);
    tmp->address = (*regIterator)->address;
    tmp->city = (*regIterator)->city;
    auto addrIterator = lower_bound(sortedByAddress.begin(), sortedByAddress.end(), tmp, compareAddress);

    Owner tempOwner = {(*regIterator)->owner_lower};
    auto ownerIndex = lower_bound(sortedByOwner.begin(), sortedByOwner.end(), tempOwner, compareOwner);
    tmp->uid = (*regIterator)->uid;
    auto inOwnerIter = lower_bound(ownerIndex->owns.begin(), ownerIndex->owns.end(), tmp, compareUID);

    delete tmp;
    delete (*addrIterator);
    ownerIndex->owns.erase(inOwnerIter);
    sortedByAddress.erase(addrIterator);
    sortedByRegion.erase(regIterator);

    return true;
}

bool CLandRegister::GetOwner(const string &city, const string &addr, string &owner) const {
    if(!existsInAddr(city, addr))
        return false;
    Land * tmp = new Land(city, addr);
    auto inAddr = lower_bound(sortedByAddress.begin(), sortedByAddress.end(), tmp, compareAddress);
    owner = (*inAddr)->owner;
    delete tmp;
    return true;
}

bool CLandRegister::GetOwner(const string &region, unsigned int id, string &owner) const {
    if(!existsInRegion(region, id))
        return false;
    Land * tmp = new Land(region, id);
    auto inReg = lower_bound(sortedByRegion.begin(), sortedByRegion.end(), tmp, compareRegion);
    owner = (*inReg)->owner;
    delete tmp;
    return true;
}

bool CLandRegister::NewOwner(const string &city, const string &addr, const string &owner) {
    if(!existsInAddr(city, addr))
        return false;

    Land * tmp = new Land(city, addr);
    auto inAddrIter = lower_bound(sortedByAddress.begin(), sortedByAddress.end(), tmp, compareAddress);

    Owner newOwner = {owner};
    if(newOwner.name == (*inAddrIter)->owner_lower){ // mixed case compare
        delete tmp;
        return false;
    }

    //find land in oldowner
    Owner oldOwner = {(*inAddrIter)->owner};
    auto ownerIndex = lower_bound(sortedByOwner.begin(), sortedByOwner.end(), oldOwner, compareOwner);
    tmp->uid = (*inAddrIter)->uid;
    auto inOwnerIter = lower_bound(ownerIndex->owns.begin(), ownerIndex->owns.end(), tmp, compareUID);
    (*inAddrIter)->setNewOwner(owner);
    ownerIndex->owns.erase(inOwnerIter);
    //find new owner
    auto newOwnerIndex = lower_bound(sortedByOwner.begin(), sortedByOwner.end(), newOwner, compareOwner);
    if(!indexOfOwner(owner, newOwnerIndex)){
        newOwnerIndex = sortedByOwner.insert(newOwnerIndex, newOwner);
    }
    (*inAddrIter)->newUID();
    newOwnerIndex->owns.push_back(*inAddrIter);

    delete tmp;
    return true;
}

bool CLandRegister::NewOwner(const string &region, unsigned int id, const string &owner) {
    if(!existsInRegion(region, id))
        return false;

    Land * tmp = new Land(region, id);
    auto inRegIter= lower_bound(sortedByRegion.begin(), sortedByRegion.end(), tmp, compareRegion);
    Owner newOwner = {owner};
    if(newOwner.name == (*inRegIter)->owner_lower){
        delete tmp;
        return false;
    }

    //find land in oldowner
    Owner oldOwner = {(*inRegIter)->owner};
    auto ownerIndex = lower_bound(sortedByOwner.begin(), sortedByOwner.end(), oldOwner, compareOwner);
    tmp->uid = (*inRegIter)->uid;
    auto inOwnerIter = lower_bound(ownerIndex->owns.begin(), ownerIndex->owns.end(), tmp, compareUID);
    ownerIndex->owns.erase(inOwnerIter);
    (*inRegIter)->setNewOwner(owner);
    //find new owner
    auto newOwnerIndex = lower_bound(sortedByOwner.begin(), sortedByOwner.end(), newOwner, compareOwner);
    if(!indexOfOwner(owner, newOwnerIndex)){
        newOwnerIndex = sortedByOwner.insert(newOwnerIndex, newOwner);
    }
    // refactor
    (*inRegIter)->newUID();
    newOwnerIndex->owns.push_back(*inRegIter);
    delete tmp;
    return true;
}

unsigned CLandRegister::Count(const string &owner) const {
    Owner tmp = Owner(owner);
    auto inOwner = lower_bound(sortedByOwner.begin(), sortedByOwner.end(), tmp, compareOwner);
    if(indexOfOwner(owner, inOwner)){
        return inOwner->owns.size();
    }
    return 0;
}


#ifndef __PROGTEST__

static void test0(void) {
    CLandRegister x;
    string owner;

    assert (x.Add("Prague", "Thakurova", "Dejvice", 12345));
    assert (x.Add("Prague", "Evropska", "Vokovice", 12345));
    assert (x.Add("Prague", "Technicka", "Dejvice", 9873));
    assert (x.Add("Plzen", "Evropska", "Plzen mesto", 78901));
    assert (x.Add("Liberec", "Evropska", "Librec", 4552));
    CIterator i0 = x.ListByAddr();
    assert (!i0.AtEnd()
            && i0.City() == "Liberec"
            && i0.Addr() == "Evropska"
            && i0.Region() == "Librec"
            && i0.ID() == 4552
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Plzen"
            && i0.Addr() == "Evropska"
            && i0.Region() == "Plzen mesto"
            && i0.ID() == 78901
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Evropska"
            && i0.Region() == "Vokovice"
            && i0.ID() == 12345
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Technicka"
            && i0.Region() == "Dejvice"
            && i0.ID() == 9873
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Thakurova"
            && i0.Region() == "Dejvice"
            && i0.ID() == 12345
            && i0.Owner() == "");
    i0.Next();
    assert (i0.AtEnd());

    assert (x.Count("") == 5);
    CIterator i1 = x.ListByOwner("");
    assert (!i1.AtEnd()
            && i1.City() == "Prague"
            && i1.Addr() == "Thakurova"
            && i1.Region() == "Dejvice"
            && i1.ID() == 12345
            && i1.Owner() == "");
    i1.Next();
    assert (!i1.AtEnd()
            && i1.City() == "Prague"
            && i1.Addr() == "Evropska"
            && i1.Region() == "Vokovice"
            && i1.ID() == 12345
            && i1.Owner() == "");
    i1.Next();
    assert (!i1.AtEnd()
            && i1.City() == "Prague"
            && i1.Addr() == "Technicka"
            && i1.Region() == "Dejvice"
            && i1.ID() == 9873
            && i1.Owner() == "");
    i1.Next();
    assert (!i1.AtEnd()
            && i1.City() == "Plzen"
            && i1.Addr() == "Evropska"
            && i1.Region() == "Plzen mesto"
            && i1.ID() == 78901
            && i1.Owner() == "");
    i1.Next();
    assert (!i1.AtEnd()
            && i1.City() == "Liberec"
            && i1.Addr() == "Evropska"
            && i1.Region() == "Librec"
            && i1.ID() == 4552
            && i1.Owner() == "");
    i1.Next();
    assert (i1.AtEnd());

    assert (x.Count("CVUT") == 0);
    CIterator i2 = x.ListByOwner("CVUT");
    assert (i2.AtEnd());

    assert (x.NewOwner("Prague", "Thakurova", "CVUT"));
    assert (x.NewOwner("Dejvice", 9873, "CVUT"));
    assert (x.NewOwner("Plzen", "Evropska", "Anton Hrabis"));
    assert (x.NewOwner("Librec", 4552, "Cvut"));
    assert (x.GetOwner("Prague", "Thakurova", owner) && owner == "CVUT");
    assert (x.GetOwner("Dejvice", 12345, owner) && owner == "CVUT");
    assert (x.GetOwner("Prague", "Evropska", owner) && owner == "");
    assert (x.GetOwner("Vokovice", 12345, owner) && owner == "");
    assert (x.GetOwner("Prague", "Technicka", owner) && owner == "CVUT");
    assert (x.GetOwner("Dejvice", 9873, owner) && owner == "CVUT");
    assert (x.GetOwner("Plzen", "Evropska", owner) && owner == "Anton Hrabis");
    assert (x.GetOwner("Plzen mesto", 78901, owner) && owner == "Anton Hrabis");
    assert (x.GetOwner("Liberec", "Evropska", owner) && owner == "Cvut");
    assert (x.GetOwner("Librec", 4552, owner) && owner == "Cvut");
    CIterator i3 = x.ListByAddr();
    assert (!i3.AtEnd()
            && i3.City() == "Liberec"
            && i3.Addr() == "Evropska"
            && i3.Region() == "Librec"
            && i3.ID() == 4552
            && i3.Owner() == "Cvut");
    i3.Next();
    assert (!i3.AtEnd()
            && i3.City() == "Plzen"
            && i3.Addr() == "Evropska"
            && i3.Region() == "Plzen mesto"
            && i3.ID() == 78901
            && i3.Owner() == "Anton Hrabis");
    i3.Next();
    assert (!i3.AtEnd()
            && i3.City() == "Prague"
            && i3.Addr() == "Evropska"
            && i3.Region() == "Vokovice"
            && i3.ID() == 12345
            && i3.Owner() == "");
    i3.Next();
    assert (!i3.AtEnd()
            && i3.City() == "Prague"
            && i3.Addr() == "Technicka"
            && i3.Region() == "Dejvice"
            && i3.ID() == 9873
            && i3.Owner() == "CVUT");
    i3.Next();
    assert (!i3.AtEnd()
            && i3.City() == "Prague"
            && i3.Addr() == "Thakurova"
            && i3.Region() == "Dejvice"
            && i3.ID() == 12345
            && i3.Owner() == "CVUT");
    i3.Next();
    assert (i3.AtEnd());

    assert (x.Count("cvut") == 3);
    CIterator i4 = x.ListByOwner("cVuT");
    assert (!i4.AtEnd()
            && i4.City() == "Prague"
            && i4.Addr() == "Thakurova"
            && i4.Region() == "Dejvice"
            && i4.ID() == 12345
            && i4.Owner() == "CVUT");
    i4.Next();
    assert (!i4.AtEnd()
            && i4.City() == "Prague"
            && i4.Addr() == "Technicka"
            && i4.Region() == "Dejvice"
            && i4.ID() == 9873
            && i4.Owner() == "CVUT");
    i4.Next();
    assert (!i4.AtEnd()
            && i4.City() == "Liberec"
            && i4.Addr() == "Evropska"
            && i4.Region() == "Librec"
            && i4.ID() == 4552
            && i4.Owner() == "Cvut");
    i4.Next();
    assert (i4.AtEnd());

    assert (x.NewOwner("Plzen mesto", 78901, "CVut"));
    assert (x.Count("CVUT") == 4);
    CIterator i5 = x.ListByOwner("CVUT");
    assert (!i5.AtEnd()
            && i5.City() == "Prague"
            && i5.Addr() == "Thakurova"
            && i5.Region() == "Dejvice"
            && i5.ID() == 12345
            && i5.Owner() == "CVUT");
    i5.Next();
    assert (!i5.AtEnd()
            && i5.City() == "Prague"
            && i5.Addr() == "Technicka"
            && i5.Region() == "Dejvice"
            && i5.ID() == 9873
            && i5.Owner() == "CVUT");
    i5.Next();
    assert (!i5.AtEnd()
            && i5.City() == "Liberec"
            && i5.Addr() == "Evropska"
            && i5.Region() == "Librec"
            && i5.ID() == 4552
            && i5.Owner() == "Cvut");
    i5.Next();
    assert (!i5.AtEnd()
            && i5.City() == "Plzen"
            && i5.Addr() == "Evropska"
            && i5.Region() == "Plzen mesto"
            && i5.ID() == 78901
            && i5.Owner() == "CVut");
    i5.Next();
    assert (i5.AtEnd());

    assert (x.Del("Liberec", "Evropska"));
    assert (x.Del("Plzen mesto", 78901));
    assert (x.Count("cvut") == 2);
    CIterator i6 = x.ListByOwner("cVuT");
    assert (!i6.AtEnd()
            && i6.City() == "Prague"
            && i6.Addr() == "Thakurova"
            && i6.Region() == "Dejvice"
            && i6.ID() == 12345
            && i6.Owner() == "CVUT");
    i6.Next();
    assert (!i6.AtEnd()
            && i6.City() == "Prague"
            && i6.Addr() == "Technicka"
            && i6.Region() == "Dejvice"
            && i6.ID() == 9873
            && i6.Owner() == "CVUT");
    i6.Next();
    assert (i6.AtEnd());

    assert (x.Add("Liberec", "Evropska", "Librec", 4552));
}

static void test1(void) {
    CLandRegister x;
    string owner;

    assert (x.Add("Prague", "Thakurova", "Dejvice", 12345));
    assert (x.Add("Prague", "Evropska", "Vokovice", 12345));
    assert (x.Add("Prague", "Technicka", "Dejvice", 9873));
    assert (!x.Add("Prague", "Technicka", "Hradcany", 7344));
    assert (!x.Add("Brno", "Bozetechova", "Dejvice", 9873));
    assert (!x.GetOwner("Prague", "THAKUROVA", owner));
    assert (!x.GetOwner("Hradcany", 7343, owner));
    CIterator i0 = x.ListByAddr();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Evropska"
            && i0.Region() == "Vokovice"
            && i0.ID() == 12345
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Technicka"
            && i0.Region() == "Dejvice"
            && i0.ID() == 9873
            && i0.Owner() == "");
    i0.Next();
    assert (!i0.AtEnd()
            && i0.City() == "Prague"
            && i0.Addr() == "Thakurova"
            && i0.Region() == "Dejvice"
            && i0.ID() == 12345
            && i0.Owner() == "");
    i0.Next();
    assert (i0.AtEnd());

    assert (x.NewOwner("Prague", "Thakurova", "CVUT"));
    assert (!x.NewOwner("Prague", "technicka", "CVUT"));
    assert (!x.NewOwner("prague", "Technicka", "CVUT"));
    assert (!x.NewOwner("dejvice", 9873, "CVUT"));
    assert (!x.NewOwner("Dejvice", 9973, "CVUT"));
    assert (!x.NewOwner("Dejvice", 12345, "CVUT"));
    assert (x.Count("CVUT") == 1);
    CIterator i1 = x.ListByOwner("CVUT");
    assert (!i1.AtEnd()
            && i1.City() == "Prague"
            && i1.Addr() == "Thakurova"
            && i1.Region() == "Dejvice"
            && i1.ID() == 12345
            && i1.Owner() == "CVUT");
    i1.Next();
    assert (i1.AtEnd());

    assert (!x.Del("Brno", "Technicka"));
    assert (!x.Del("Karlin", 9873));
    assert (x.Del("Prague", "Technicka"));
    assert (!x.Del("Prague", "Technicka"));
    assert (!x.Del("Dejvice", 9873));
}

int main(void) {
   test0();
    test1();

    return 0;
}

#endif /* __PROGTEST__ */
