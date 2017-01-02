#### JsonProperty a simple C++ framework which allows to map JSON to C++ structure

It is tested with VS2015 and Clang (LLVM 2014).

There are several C++ frameworks which allow parse and access JSON.   

JsonProperty is a framework which allows to map JSON to C++. 

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
