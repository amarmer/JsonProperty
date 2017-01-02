#### JsonProperty a simple C++ framework which allows to model JSON via C++ structure

It is tested with VS2015 and Clang (LLVM 2014).

There are several C++ frameworks which allow to parse and access JSON.   

JsonProperty is a framework which allows to model JSON to C++.
To parse and access JSON, used popular "jsoncpp" framework https://github.com/open-source-parsers/jsoncpp without modifications, 
(but could be used any other framework).

Any class which needs to model JSON should be derived from Json::Data.

Any member of the class which needs to bind JSON element should be declared like 'JsonProperty<type> member{key(jsonKey)};' or for simplicity via macro 'JSON_PROPERTY(type, member, jsonKey)'. 

For instance:
```C++
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
```

How it can be used:
```C++
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
        "salary": 70000
        }
    }
    */
```