#include "JsonProperty.h"

using namespace std;

struct Employee : public Json::Data
{
    Employee() {}
    Employee(const string& name, int salary)
    {
        name_ = name;
        salary_ = salary;
    }

    JSON_PROPERTY(string, name_, "name");
    JSON_PROPERTY(int, salary_, "salary");
};

struct Manager : public Employee
{
    using Employee::Employee;

    JSON_PROPERTY(vector<Employee>, employees_, "employees");
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
    Employee empl1("A", 50000);
    Employee empl2("B", 55000);

    Manager manager("X", 70000);
    manager.employees_ = {empl1 , empl2};

    // Change manager's last name
    manager.name_ = "Z";

    Department department("HR", manager);

    // Add employee
    Employee employee3;
    employee3.init("{\"name\":\"C\",\"salary\":57000}");

    vector<Employee> employees = manager.employees_;
    employees.push_back(employee3);

    department.manager_.employees_ = employees;

    // 'Manager' reference (when changed, 'department' data is updated)
    Manager& managerReference = department.manager_;
    managerReference.salary_ = 75000;

    // 'Manager' copy (when changed, it won't affect 'department')
    Manager managerCopy = department.manager_;
    managerCopy.salary_ = 100000;
 
    string str = department.toString();
    /*
    JSON of 'department':
    {
    "department": "HR",
    "manager": {
        "employees": [
            { "name": "A", "salary": 50000}, 
            { "name": "B", "salary": 55000}, 
            { "name": "C", "salary": 57000}
        ],
        "name": "Z",
        "salary": 75000
        }
    }
    */

    return 0;
}

