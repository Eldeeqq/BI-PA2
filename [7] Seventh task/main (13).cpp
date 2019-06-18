#ifndef __PROGTEST__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <list>
#include <algorithm>

#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

#endif /* C++ 11 */

using namespace std;
#endif /* __PROGTEST__ */

class NoRouteException {
};

template<typename E>
class defFunctor {
public:
    bool operator()(const E &val) const {
        return true;
    }
};

template<typename _T, typename _E>
class CRoute {
public:

    map<_T, multimap<_T, _E>> graph;

    bool addEntry(const _T &a, const _T &b, const _E &c);

    // default constructor
    CRoute<_T, _E>() = default;

    // destructor
    ~CRoute<_T, _E>() = default;

    // Add
    CRoute<_T, _E> &Add(const _T &a, const _T &b, const _E &c);

    // Find (with optional filter)
    template<typename C=defFunctor<_E>>
    list<_T> Find(const _T &from, const _T &to, const C &comp = C()) const;
};

template<typename _T, typename _E>
bool CRoute<_T, _E>::addEntry(const _T &a, const _T &b, const _E &c) {
    if (graph.find(a) == graph.end()) {
        graph.emplace(a, multimap<_T, _E>());
    }
    graph.at(a).emplace(b, c);
    return true;
}

template<typename _T, typename _E>
CRoute<_T, _E> &CRoute<_T, _E>::Add(const _T &a, const _T &b, const _E &c) {
    addEntry(a, b, c);
    addEntry(b, a, c);
    return *this;
}


template<typename _T, typename _E>
template<typename C>
list<_T> CRoute<_T, _E>::Find(const _T &from, const _T &to, const C &comp) const {
    list<_T> edges;
    queue<_T>peaks;
    list<pair<_T, _T>> routes;
    set<_T> visited;


    if(from == to){
        edges.push_back(from);
        return edges;
    }

    auto x = graph.find(from);
    if (x == graph.end())
        throw NoRouteException();

    routes.emplace_back(from, from);

    peaks.push(x->first);
    visited.emplace(x->first);
    int cnt = 0;
    while (!peaks.empty()) {
        _T first = peaks.front();
        peaks.pop();
        for (auto edge : graph.at(first)) {
            if (visited.end() == visited.find(edge.first)) {
                auto range = graph.at(first).equal_range(edge.first);

                while (range.first!= graph.at(first).end() && range.first->first == edge.first) {

                    if (comp(range.first->second)) {

                        peaks.emplace(edge.first);
                        visited.emplace(edge.first);
                        routes.emplace_back(first, edge.first);
                    }
                    range.first++;
                }

            }

        }

        cnt++;
        if (visited.find(to) != visited.end())
            break;
    }



    _T desired = to;
    while(!routes.empty()){
        if(routes.back().second == desired){
        edges.insert(edges.begin(), routes.back().second);
        desired = routes.back().first;
        }
        routes.pop_back();
    }

    if(edges.empty())
        throw NoRouteException();

    return edges;
}


#ifndef __PROGTEST__

//=================================================================================================
class CTrain {
public:
    CTrain(const string &company,
           int speed)
            : m_Company(company),
              m_Speed(speed) {
    }

    //---------------------------------------------------------------------------------------------
    string m_Company;
    int m_Speed;
};

//=================================================================================================
class TrainFilterCompany {
public:
    TrainFilterCompany(const set<string> &companies)
            : m_Companies(companies) {
    }

    //---------------------------------------------------------------------------------------------
    bool operator()(const CTrain &train) const {
        return m_Companies.find(train.m_Company) != m_Companies.end();
    }
    //---------------------------------------------------------------------------------------------
private:
    set<string> m_Companies;
};

//=================================================================================================
class TrainFilterSpeed {
public:
    TrainFilterSpeed(int min,
                     int max)
            : m_Min(min),
              m_Max(max) {
    }

    //---------------------------------------------------------------------------------------------
    bool operator()(const CTrain &train) const {
        return train.m_Speed >= m_Min && train.m_Speed <= m_Max;
    }
    //---------------------------------------------------------------------------------------------
private:
    int m_Min;
    int m_Max;
};

//=================================================================================================
bool NurSchnellzug(const CTrain &zug) {
    return (zug.m_Company == "OBB" || zug.m_Company == "DB") && zug.m_Speed > 100;
}

//=================================================================================================
static string toText(const list<string> &l) {
    ostringstream oss;

    auto it = l.cbegin();
    oss << *it;
    for (++it; it != l.cend(); ++it)
        oss << " > " << *it;
    return oss.str();
}

//=================================================================================================
int main(void) {
    CRoute<string, CTrain> lines;

    lines.Add("Berlin", "Prague", CTrain("DB", 120))
            .Add("Berlin", "Prague", CTrain("CD", 80))
            .Add("Berlin", "Dresden", CTrain("DB", 160))
            .Add("Dresden", "Munchen", CTrain("DB", 160))
            .Add("Munchen", "Prague", CTrain("CD", 90))
            .Add("Munchen", "Linz", CTrain("DB", 200))
            .Add("Munchen", "Linz", CTrain("OBB", 90))
            .Add("Linz", "Prague", CTrain("CD", 50))
            .Add("Prague", "Wien", CTrain("CD", 100))
            .Add("Linz", "Wien", CTrain("OBB", 160))
            .Add("Paris", "Marseille", CTrain("SNCF", 300))
            .Add("Paris", "Dresden", CTrain("SNCF", 250));

    list<string> r1 = lines.Find("Berlin", "Linz");
    assert (toText(r1) == "Berlin > Prague > Linz");

    list<string> r2 = lines.Find("Linz", "Berlin");
    assert (toText(r2) == "Linz > Prague > Berlin");

    list<string> r3 = lines.Find("Wien", "Berlin");
    assert (toText(r3) == "Wien > Prague > Berlin");

    list<string> r4 = lines.Find("Wien", "Berlin", NurSchnellzug);
    assert (toText(r4) == "Wien > Linz > Munchen > Dresden > Berlin");

    list<string> r5 = lines.Find("Wien", "Munchen", TrainFilterCompany(set<string>{"CD", "DB"}));
    assert (toText(r5) == "Wien > Prague > Munchen");

    list<string> r6 = lines.Find("Wien", "Munchen", TrainFilterSpeed(120, 200));
    assert (toText(r6) == "Wien > Linz > Munchen");

    list<string> r7 = lines.Find("Wien", "Munchen", [](const CTrain &x) { return x.m_Company == "CD"; });
    assert (toText(r7) == "Wien > Prague > Munchen");

    list<string> r8 = lines.Find("Munchen", "Munchen");
    assert (toText(r8) == "Munchen");

    list<string> r9 = lines.Find("Marseille", "Prague");
    assert (toText(r9) == "Marseille > Paris > Dresden > Berlin > Prague"
            || toText(r9) == "Marseille > Paris > Dresden > Munchen > Prague");

    try {
        list<string> r10 = lines.Find("Marseille", "Prague", NurSchnellzug);
        assert ("Marseille > Prague connection does not exist!!" == NULL);
    }
    catch (const NoRouteException &e) {
    }
    list<string> r11 = lines.Find("Salzburg", "Salzburg");
    assert (toText(r11) == "Salzburg");

    list<string> r12 = lines.Find("Salzburg", "Salzburg", [](const CTrain &x) { return x.m_Company == "SNCF"; });
    assert (toText(r12) == "Salzburg");

    try {
        list<string> r13 = lines.Find("London", "Oxford");
        assert ("London > Oxford connection does not exist!!" == NULL);
    }
    catch (const NoRouteException &e) {
    }
    return 0;
}

#endif  /* __PROGTEST__ */
