#### C# JsonProperty implementation in C++

C# has JsonProperty attribute, which allows to model JSON in C#, for instance:

```C++
public class Person
{
    [JsonProperty("name")]
    public string Name { get; set; }
}
```

This is how C++ class looks like:

```C++
class Person: public Json::Data
{
public:
    JSON_PROPERTY(string, Name, "name");
}
```

JsonProperty.h can be implemented by using any C++ JSON library (which allows parse and access JSON) can be extended to implement C# JsonProerty functionality. As an example is implemented by using a popular C++ JSON library "jsoncpp" https://github.com/open-source-parsers/jsoncpp (json.h and jsoncpp.cpp), which allows to parse and access JSON.

Bellow is an example of how C++ JsonProperty can be used.

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

struct Department : public Json::Data
{
    Department(const string& name, const Employee& manager, const vector<Employee>& employees)
    {
        name_ = name;
        manager_ = manager;
        employees_ = employees;
    }

    JSON_PROPERTY(string, name_, "department");
    JSON_PROPERTY(Employee, manager_, "manager");
    JSON_PROPERTY(vector<Employee>, employees_, "employees");
};
```

Example of how these classes can be used:
```C++
void main()
{
    Employee empl1("A", "K", 50000);
    Employee empl2("B", "L", 55000);

    Employee manager("A", "X", 70000);

    // Change manager's last name
    manager.lastName_ = "Z";

    Department department("HR", manager, { empl1, empl2 });

    // Add employee
    Employee empl3;
    empl3.init("{\"firstName\":\"C\",\"lastName\":\"M\",\"salary\":57000}");

    vector<Employee> employees = department.employees_;
    employees.push_back(empl3);

    department.employees_ = employees;

    string str = department.toStyledString();
    /*
    JSON of 'department':
    {
        "department" : "HR",
        "employees" : [
            { "firstName" : "A", "lastName" : "K", "salary" : 50000 },
            { "firstName" : "B", "lastName" : "L", "salary" : 55000 },
            { "firstName" : "C", "lastName" : "M", "salary" : 57000 }
        ],
        "manager" : { "firstName" : "A", "lastName" : "Z", "salary" : 70000 }
    }    
    */

    struct EmployeesByFirstName : public Json::Data
    {
        JSON_PROPERTY(unordered_map<string COMMA vector<Employee>>, mapFirstNameEployees_, "firstNameEployees");
    };

    EmployeesByFirstName employeesByFirstName;
    employeesByFirstName.mapFirstNameEployees_ = {
        {"A", {empl1, department.manager_}}, {"B", {empl2}}, {"C", {empl3}}
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
