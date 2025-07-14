#ifndef RECORD_H
#define RECORD_H

#include <any>
#include <cstddef>
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;

class Record {
private:
  class Attribute {
  public:
    string name;
    string type;
    any value;

    Attribute(string name, string type, string strVal)
        : name(name), type(type) {
      if (type == "Double")
        value = stod(strVal);
      else if (type == "Integer")
        value = stoi(strVal);
      else {
        type = "String"; // Force to String if user provides unknown type
        value = strVal;
      }
    }

    friend ostream &operator<<(ostream &os, const Attribute &a) {
      if (a.type == "Double")
        os << any_cast<double>(a.value);
      else if (a.type == "Integer")
        os << any_cast<int>(a.value);
      else
        os << any_cast<string>(a.value);
      return os;
    }
  };

public:
  vector<Attribute> attributes;
  // Write code to store/handle an arbitrary number of
  //   - Attribute Names
  //   - Attribute Types
  //   - Attribute Values

  Record() {}

  Record(const vector<string> &names, const vector<string> &types,
         const vector<string> &vals) {
    for (size_t i = 0; i < names.size(); i++) {
      addAttribute(names[i], types[i], vals[i]);
    }
  }

  void addAttribute(const string &name, const string &type,
                    const string &value) {
    attributes.emplace_back(name, type, value);
  }

  void addAttribute(const Attribute &attr) { attributes.push_back(attr); }

  Attribute getAttribute(const string &name) const {
    size_t i;
    for (i = 0; i < attributes.size(); i++) {
      if (name == attributes[i].name)
        break;
    }
    return attributes[i];
  }

  // Function to get the vector attrNames
  vector<string> get_attrNames() {
    vector<string> attrNames;

    for (Attribute attr : attributes) {
      attrNames.push_back(attr.name);
    }
    return attrNames;
  }

  // Function to get the vector attrNames
  vector<string> get_attrTypes() {
    vector<string> attrTypes;

    for (Attribute attr : attributes) {
      attrTypes.push_back(attr.type);
    }
    return attrTypes;
  }

  friend ostream &operator<<(ostream &os, const Record &r) {
    bool first = true;
    for (Attribute attr : r.attributes) {
      if (first)
        first = false;
      else
        cout << " ";

      cout << attr;
    }
    return os;
  }

  vector<Attribute> getAttributes() const { return attributes; }
};

#endif
