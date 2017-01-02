#include "JsonProperty.h"

using namespace std;

struct Employee : public Json::Data
{
    Employee() {}
    Employee(const string& firstName, const string& lastName, int salary)
    {
        firstName_ = firstName;
        lastName_ = lastName;
        salary_ = salary;
    }

    JSON_PROPERTY(string, firstName_, "firstName");
    JSON_PROPERTY(string, lastName_, "lastName");
    JSON_PROPERTY(int, salary_, "salary");
};

struct Manager : public Json::Data
{
    JSON_PROPERTY(Employee, me_, "info");
    JSON_PROPERTY(vector<Employee>, subordinates_, "subordinates");
};

struct Department : public Json::Data
{
    Department(const string& name, const Manager& manager)
    {
        name_ = name;
        manager_ = manager;
    }

    JSON_PROPERTY(string, name_, "department");
    JSON_PROPERTY(Manager, manager_, "manager");
};


int main(int argc, char* argv[])
{
    Employee empl1("A", "K", 50000);
    Employee empl2("B", "L", 55000);

    Manager manager;
    manager.me_ = Employee("A", "X", 70000);
    manager.subordinates_ = {empl1 , empl2};

    // Change manager's last name
    manager.me_.lastName_ = "Z";

    Department department("HR", manager);

    // Add employee
    Employee empl3;
    empl3.init("{\"firstName\":\"C\",\"lastName\":\"M\",\"salary\":57000}");

    vector<Employee> subordinates = manager.subordinates_;
    subordinates.push_back(empl3);

    department.manager_.subordinates_ = subordinates;

    // 'Manager' reference (when changed, 'department' data is updated)
    Manager& managerReference = department.manager_;
    managerReference.me_.salary_ = 75000;

    // 'Manager' copy (when changed, it won't affect 'department')
    Manager managerCopy = department.manager_;
    managerCopy.me_.salary_ = 100000;

    string str = department.toStyledString();
    /*
    JSON of 'department':
    {
        "department" : "HR",
        "manager" : {
            "info" : { "firstName" : "A", "lastName" : "Z", "salary" : 75000 },
            "subordinates" : [
                { "firstName" : "A", "lastName" : "K", "salary" : 50000 },
                { "firstName" : "B", "lastName" : "L", "salary" : 55000 },
                { "firstName" : "C", "lastName" : "M", "salary" : 57000 }
            ]
        }
    }    
    */

    struct EmployeesByFirstName : public Json::Data
    {
        JSON_PROPERTY(map<string COMMA vector<Employee>>, mapFirstNameEployees_, "firstNameEployees");
    };

    EmployeesByFirstName employeesByFirstName;
    employeesByFirstName.mapFirstNameEployees_ = { {"A", {empl1, department.manager_.me_}},  {"B", {empl2}}, {"C", {empl3}} };

    str = employeesByFirstName.toStyledString();
    /*
    JSON of 'employeesByFirstName':
    { 
        "firstNameEployees" : {
            "A" : [
                { "firstName" : "A", "lastName" : "K", "salary" : 50000 },
                { "firstName" : "A", "lastName" : "Z", "salary" : 75000 }
            ],
            "B" : [ { "firstName" : "B", "lastName" : "L", "salary" : 55000 } ],
            "C" : [ { "firstName" : "C", "lastName" : "M", "salary" : 57000 } ]
            }
    }
    */

    return 0;
}

