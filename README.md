#### JsonProperty is a simple C++ framework which allows to model JSON via C++ structure

It is tested with VS2015 and Clang (LLVM 2014).

There are several C++ frameworks which allow to parse and access JSON.   

JsonProperty is a framework which allows to model JSON via C++. It is similar to C# JsonProperty.

To parse and access JSON, used popular "jsoncpp" framework https://github.com/open-source-parsers/jsoncpp without modifications (but could be used any other framework).

Any class which needs to model JSON should be derived from Json::Data.

Any member of the class which needs to bind JSON element should be declared like ```JsonProperty<type> m{key(jsonKey)};``` or for simplicity via macro ```JSON_PROPERTY(type, m, jsonKey)```. 

For instance:
```C++
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
```

Example of how these classes can be used:
```C++
void main()
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
    employeesByFirstName.mapFirstNameEployees_ = { 
        {"A", {empl1, department.manager_.me_}},  {"B", {empl2}}, {"C", {empl3}} 
    };

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
} 
```
