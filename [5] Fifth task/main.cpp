#ifndef __PROGTEST__

#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>

using namespace std;

class CTimeStamp {
public:
    CTimeStamp(int year, int month, int day, int hour, int minute, double sec)
            : year(year), month(month), day(day), hour(hour), minute(minute), sec(sec) {}

    int Compare(const CTimeStamp &x) const {
        if (year > x.year) return 1;
        if (year < x.year) return -1;

        if (month > x.month) return 1;
        if (month < x.month) return -1;

        if (day > x.day) return 1;
        if (day < x.day) return -1;

        if (hour > x.hour) return 1;
        if (hour < x.hour) return -1;

        if (minute > x.minute) return 1;
        if (minute < x.minute) return -1;

        if (sec > x.sec) return 1;
        if (sec < x.sec) return -1;

        return 0;
    }

    friend ostream &operator<<(ostream &os,
                               const CTimeStamp &x);

private:
    int year, month, day, hour, minute;
    double sec;
};

ostream &operator<<(ostream &os, const CTimeStamp &x) {
    os << x.year << "-";
    os << setw(2) << setfill('0') << x.month << "-";
    os << setw(2) << setfill('0') << x.day << " ";
    os << setw(2) << setfill('0') << x.hour << ":";
    os << setw(2) << setfill('0') << x.minute << ":";
    os << fixed << setprecision(3) << x.sec;
    return os;
}

class CMail {
public:
    CMail(const CTimeStamp &timeStamp, const string &from, const string &to, const string &subject)
            : ts(timeStamp), from(from), to(to), subject(subject) {}

    int CompareByTime(const CTimeStamp &x) const {
        return ts.Compare(x);
    }

    int CompareByTime(const CMail &x) const {
        return ts.Compare(x.ts);
    }

    const string &From(void) const {
        return from;
    }

    const string &To(void) const {
        return to;
    }

    const string &Subject(void) const {
        return subject;
    }

    const CTimeStamp &TimeStamp(void) const {
        return ts;
    }

    friend ostream &operator<<(ostream &os,
                               const CMail &x) {
        os << x.ts << " " << x.from << " -> " << x.to << ", subject: " << x.subject;
        return os;
    }

private:
    CTimeStamp ts;
    string from, to, subject;
};

// your code will be compiled in a separate namespace
namespace MysteriousNamespace {
#endif /* __PROGTEST__ */

//----------------------------------------------------------------------------------------

    // map used to determine month number in gregorian calendar
    map<string, int> months = {{"Jan", 1},
                               {"Feb", 2},
                               {"Mar", 3},
                               {"Apr", 4},
                               {"May", 5},
                               {"Jun", 6},
                               {"Jul", 7},
                               {"Aug", 8},
                               {"Sep", 9},
                               {"Oct", 10},
                               {"Nov", 11},
                               {"Dec", 12}};

    int getNumberOfMonth(const string &mon) {
        return months[mon];
    }

    struct MailCompare {
        bool operator()(const CMail &l, const CMail &r) const {
            return l.CompareByTime(r) < 0;
        }
    };

    class CMailLog {
    public:
        int ParseLog(istream &in);
        list<CMail> ListMail(const CTimeStamp &from, const CTimeStamp &to) const;
        set<string> ActiveUsers(const CTimeStamp &from, const CTimeStamp &to) const;

    private:
        bool parseLine(string &line);
        multiset<CMail, MailCompare> mails;
        map<string, string> sender;
        map<string, string> subject;

        // private member functions
        bool newSubject(const string & key, const string & subj);
        bool newSender(const string & key, const string & send);
        bool newMail(const string & key, const CTimeStamp & time, const string & recipient);
        bool stringCutter(string & line, const char & delimiter, int cycles = 1, int spacer = 1);
    };

    int CMailLog::ParseLog(istream &in) {
        int counter = 0;
        string holder;
        while (getline(in, holder)) {
            if (parseLine(holder)) counter++;
        }
        return counter;
    }

    list<CMail> CMailLog::ListMail(const CTimeStamp &from, const CTimeStamp &to) const {
        const CMail low(from, "", "", "");
        const CMail up(to, "", "", "");
        list<CMail> lst;

        auto lower = mails.lower_bound(low);
        if(lower == mails.end() || lower->CompareByTime(to)>0)
            return lst;
        while(lower!=mails.end() && lower->CompareByTime(to)<=0){
            lst.push_back(*lower);
            lower++;
        }
        return lst;
    }

    set<string> CMailLog::ActiveUsers(const CTimeStamp &from, const CTimeStamp &to) const {
        const CMail low(from, "", "", "");
        const CMail up(to, "", "", "");
        set<string> users;

        auto lower = mails.lower_bound(low);
        if(lower == mails.end() || lower->CompareByTime(to)>0)
            return users;
        while(lower!=mails.end() && lower->CompareByTime(to)<=0){
            users.insert(lower->From());
            users.insert(lower->To());
            lower++;
        }
        return users;
    }

    bool CMailLog::parseLine(string &line) {
        int month = getNumberOfMonth(line.substr(0, line.find_first_of(' ')));
        if (month == 0) return false;
        stringCutter(line, ' ');

        int day = atoi(line.substr(0, line.find_first_of(' ')).c_str());

        stringCutter(line, ' ');
        int year = atoi(line.substr(0, line.find_first_of(' ')).c_str());

        stringCutter(line, ' ');
        int hour = atoi(line.substr(0, line.find_first_of(':')).c_str());

        stringCutter(line, ':');
        int min = atoi(line.substr(0, line.find_first_of(':')).c_str());

        stringCutter(line, ':');
        double sec = stod(line.substr(0, line.find_first_of(' ')).c_str());

        stringCutter(line, ' ', 2);
        string uid = line.substr(0, line.find_first_of(':'));
        stringCutter(line, ':', 1, 2);


        int eqIndex = line.find_first_of('=');
        if (eqIndex == -1)
            return false;

        string method = line.substr(0, line.find_first_of('='));
        if (method == "to") {

            line.erase(0, eqIndex + 1);
            CTimeStamp timeStamp(year, month, day, hour, min, sec);
            newMail(uid, timeStamp, line);
            return true;

        } else if (method == "from") {

            line.erase(0, eqIndex + 1);
            newSender(uid, line);
            return false;

        } else if (method == "subject") {

            line.erase(0, eqIndex + 1);
            newSubject(uid, line);
            return false;
        }

        return false;
    }

    bool CMailLog::newSubject(const string &key, const string & subj) {
        subject[key] = subj;
        return true;
    }

    bool CMailLog::newSender(const string &key, const string &send) {
        sender.insert(std::pair<string, string>{key, send});
        subject.insert(std::pair<string, string>{key, ""});
        return false;
    }

    bool CMailLog::newMail(const string &key, const CTimeStamp &time, const string &recipient) {
        auto from = sender[key];
        auto sub = subject[key];
        mails.insert(CMail(time, from, recipient, sub));
        return false;
    }

    bool CMailLog::stringCutter(string &line, const char &delimeter, int cycles, int spacer) {
        for(int i = 0; i<cycles; i++)
            line.erase(0, line.find_first_of(delimeter)+spacer);
        return true;
    }
//----------------------------------------------------------------------------------------
#ifndef __PROGTEST__
} // namespace
string printMail(const list<CMail> &all) {
    ostringstream oss;
    for (const auto &mail : all)
        oss << mail << endl;
    return oss.str();
}

string printUsers(const set<string> &all) {
    ostringstream oss;
    bool first = true;
    for (const auto &name : all) {
        if (!first)
            oss << ", ";
        else
            first = false;
        oss << name;
    }
    return oss.str();
}

int main(void) {


    MysteriousNamespace::CMailLog m;
    list<CMail> mailList;
    set<string> users;
    istringstream iss;

    iss.clear();
    iss.str(
            "Mar 29 2019 12:35:32.233 relay.fit.cvut.cz ADFger72343D: from=user1@fit.cvut.cz\n"
            "Mar 29 2019 12:37:16.234 relay.fit.cvut.cz JlMSRW4232Df: from=person3@fit.cvut.cz\n"
            "Mar 29 2019 12:55:13.023 relay.fit.cvut.cz JlMSRW4232Df: subject=New progtest homework!\n"
            "Mar 29 2019 13:38:45.043 relay.fit.cvut.cz Kbced342sdgA: from=office13@fit.cvut.cz\n"
            "Mar 29 2019 13:36:13.023 relay.fit.cvut.cz JlMSRW4232Df: to=user76@fit.cvut.cz\n"
            "Mar 29 2019 13:55:31.456 relay.fit.cvut.cz KhdfEjkl247D: from=PR-department@fit.cvut.cz\n"
            "Mar 29 2019 14:18:12.654 relay.fit.cvut.cz Kbced342sdgA: to=boss13@fit.cvut.cz\n"
            "Mar 29 2019 14:48:32.563 relay.fit.cvut.cz KhdfEjkl247D: subject=Business partner\n"
            "Mar 29 2019 14:58:32.000 relay.fit.cvut.cz KhdfEjkl247D: to=HR-department@fit.cvut.cz\n"
            "Mar 29 2019 14:25:23.233 relay.fit.cvut.cz ADFger72343D: mail undeliverable\n"
            "Mar 29 2019 15:02:34.231 relay.fit.cvut.cz KhdfEjkl247D: to=CIO@fit.cvut.cz\n"
            "Mar 29 2019 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=CEO@fit.cvut.cz\n"
            "Mar 29 2019 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=dean@fit.cvut.cz\n"
            "Mar 29 2019 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=vice-dean@fit.cvut.cz\n"
            "Mar 29 2019 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=archive@fit.cvut.cz\n");
    assert (m.ParseLog(iss) == 8);
    mailList = m.ListMail(CTimeStamp(2019, 3, 28, 0, 0, 0),
                          CTimeStamp(2019, 3, 29, 23, 59, 59));
    assert (printMail(mailList) ==
            "2019-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!\n"
            "2019-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz, subject: \n"
            "2019-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner\n"
            "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> CEO@fit.cvut.cz, subject: Business partner\n"
            "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> dean@fit.cvut.cz, subject: Business partner\n"
            "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> vice-dean@fit.cvut.cz, subject: Business partner\n"
            "2019-03-29 15:02:34.230 PR-department@fit.cvut.cz -> archive@fit.cvut.cz, subject: Business partner\n"
            "2019-03-29 15:02:34.231 PR-department@fit.cvut.cz -> CIO@fit.cvut.cz, subject: Business partner\n");
    mailList = m.ListMail(CTimeStamp(2019, 3, 28, 0, 0, 0),
                          CTimeStamp(2019, 3, 29, 14, 58, 32));
    assert (printMail(mailList) ==
            "2019-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!\n"
            "2019-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz, subject: \n"
            "2019-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner\n");
    mailList = m.ListMail(CTimeStamp(2019, 3, 30, 0, 0, 0),
                          CTimeStamp(2019, 3, 30, 23, 59, 59));
    assert (printMail(mailList) == "");
    users = m.ActiveUsers(CTimeStamp(2019, 3, 28, 0, 0, 0),
                          CTimeStamp(2019, 3, 29, 23, 59, 59));
    assert (printUsers(users) ==
            "CEO@fit.cvut.cz, CIO@fit.cvut.cz, HR-department@fit.cvut.cz, PR-department@fit.cvut.cz, archive@fit.cvut.cz, boss13@fit.cvut.cz, dean@fit.cvut.cz, office13@fit.cvut.cz, person3@fit.cvut.cz, user76@fit.cvut.cz, vice-dean@fit.cvut.cz");
    users = m.ActiveUsers(CTimeStamp(2019, 3, 28, 0, 0, 0),
                          CTimeStamp(2019, 3, 29, 13, 59, 59));
    assert (printUsers(users) == "person3@fit.cvut.cz, user76@fit.cvut.cz");
    return 0;
}

#endif /* __PROGTEST__ */
