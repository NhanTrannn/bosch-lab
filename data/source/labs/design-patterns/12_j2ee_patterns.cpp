/*
 * Lab: Enterprise (J2EE) patterns in C++: MVC, DAO + Transfer Object, Business Delegate + Service Locator
 * Topic: Design patterns — J2EE
 * Predict before running: What does the view print before and after the controller changes the model? What does the DAO hold after update and delete? How many times is each service looked up "from the registry" when the delegate is called three times?
 * Key idea: MVC separates Model (data), View (presentation) and Controller (takes input, updates the model, refreshes the view). DAO hides how data is stored behind an interface (getAll/get/update/delete); a Transfer Object is a plain data object passed between layers. Business Delegate decouples the presentation tier from business services, and Service Locator finds services (an expensive lookup) and caches them.
 * Try next: Add a second view (e.g. a JSON view) driven by the same controller. Replace the in-memory DAO with one that writes to a file — does any caller change?
 */
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ---- MVC
struct Student { string rollNo, name; };                    // model (also used as a transfer object)
struct StudentView {
    void print(const Student &s) const { cout << "  view: Student " << s.name << " (roll no " << s.rollNo << ")\n"; }
};
class StudentController {
public:
    StudentController(Student m, StudentView v) : model(move(m)), view(v) {}
    void setName(const string &n) { model.name = n; }
    void updateView() const { view.print(model); }
private:
    Student model;
    StudentView view;
};

// ---- DAO
struct StudentDao {
    virtual ~StudentDao() = default;
    virtual vector<Student> getAll() const = 0;
    virtual void update(const Student &s) = 0;
    virtual void remove(const string &rollNo) = 0;
};
class InMemoryStudentDao : public StudentDao {
public:
    InMemoryStudentDao() : rows{{"0", "Robert"}, {"1", "John"}} {}
    vector<Student> getAll() const override { return rows; }
    void update(const Student &s) override
    {
        for (auto &r : rows) if (r.rollNo == s.rollNo) r.name = s.name;
        cout << "  dao: student " << s.rollNo << " updated\n";
    }
    void remove(const string &rollNo) override
    {
        rows.erase(remove_if(rows.begin(), rows.end(), [&](const Student &r) { return r.rollNo == rollNo; }), rows.end());
        cout << "  dao: student " << rollNo << " deleted\n";
    }
private:
    vector<Student> rows;
};
void listAll(const StudentDao &dao)
{
    cout << "  all:";
    for (auto &s : dao.getAll()) cout << " [" << s.rollNo << ' ' << s.name << ']';
    cout << '\n';
}

// ---- business delegate + service locator
struct BusinessService {
    virtual ~BusinessService() = default;
    virtual void process() = 0;
};
struct EjbService : BusinessService { void process() override { cout << "  processing in the EJB service\n"; } };
struct JmsService : BusinessService { void process() override { cout << "  processing in the JMS service\n"; } };
class ServiceLocator {
public:
    static shared_ptr<BusinessService> get(const string &name)
    {
        static map<string, shared_ptr<BusinessService>> cache;
        if (auto it = cache.find(name); it != cache.end()) { cout << "  locator: " << name << " from cache\n"; return it->second; }
        cout << "  locator: looking up " << name << " in the registry\n";   // the expensive part
        shared_ptr<BusinessService> s = name == "EJB" ? shared_ptr<BusinessService>(make_shared<EjbService>()) : make_shared<JmsService>();
        return cache[name] = s;
    }
};
class BusinessDelegate {                  // what the presentation tier talks to
public:
    void setServiceType(string t) { type = move(t); }
    void doTask() const { ServiceLocator::get(type)->process(); }
private:
    string type = "EJB";
};

int main()
{
    cout << "MVC:\n";
    StudentController controller({"10", "Robert"}, StudentView{});
    controller.updateView();
    controller.setName("John");
    controller.updateView();

    cout << "DAO + transfer object:\n";
    InMemoryStudentDao dao;
    listAll(dao);
    Student s = dao.getAll()[0];          // a transfer object: a copy of the row
    s.name = "Michael";
    dao.update(s);
    dao.remove("1");
    listAll(dao);

    cout << "Business delegate + service locator:\n";
    BusinessDelegate delegate;
    delegate.doTask();
    delegate.setServiceType("JMS");
    delegate.doTask();
    delegate.setServiceType("EJB");
    delegate.doTask();
    return 0;
}
