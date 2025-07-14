#ifndef SELECT_H
#define SELECT_H

#include "FileReader.h"
#include "Operator.h"
#include <any>
#include <vector>

class Select : public Operator {
private:
  Operator *input;
  FileReader fr;
  string p_attr;
  string p_op;
  string p_val;
  int index = 0;
  vector<Record> inPage;

public:
  // Select constructors
  Select(string fileName, string p_attr, string p_op, string p_val)
      : fr(fileName), input(&fr), p_attr(p_attr), p_op(p_op), p_val(p_val) {}

  Select(Operator *op, string p_attr, string p_op, string p_val)
      : input(op), p_attr(p_attr), p_op(p_op), p_val(p_val) {}

  void open() { input->open(); }

  vector<Record> next() {
    vector<Record> outPage;

    // While outPage is not full
    while (outPage.size() < pageSize) {
      // Get a new page if current page is full
      if (index >= inPage.size()) {
        inPage = input->next();
        index = 0;

        // Break if they are no more pages
        if (inPage.empty())
          break;
      }

      // Process the current record
      Record inRecord = inPage[index];
      any attr_value = inRecord.getAttribute(p_attr).value;

      // If the condition is satisfied, add the record to the outPage
      if (evaluate_condition(attr_value, p_op, p_val)) {
        outPage.push_back(inRecord);
      }

      // Move the index to the next record
      index++;
    }

    return outPage;
  }

  void close() { input->close(); }

  // Function to validate the condition
  bool evaluate_condition(any attr_value, string comp_op, string value) {

    // Check if the attribute value has the same type as the raw value
    // Compare values based on their types
    if (attr_value.type() == typeid(int)) {
      int attr_val = any_cast<int>(attr_value);
      int cmp_val = stoi(value);

      if (comp_op == "<")
        return attr_val < cmp_val;

      if (comp_op == ">")
        return attr_val > cmp_val;

      if (comp_op == "=")
        return attr_val == cmp_val;

      if (comp_op == "!=")
        return attr_val != cmp_val;
    }

    else if (attr_value.type() == typeid(double)) {
      double attr_val = any_cast<double>(attr_value);
      double cmp_val = stod(value);

      if (comp_op == "<")
        return attr_val < cmp_val;

      if (comp_op == ">")
        return attr_val > cmp_val;

      if (comp_op == "=")
        return attr_val == cmp_val;

      if (comp_op == "!=")
        return attr_val != cmp_val;
    }

    else if (attr_value.type() == typeid(string)) {
      string attr_val = any_cast<string>(attr_value);

      if (comp_op == "<")
        return attr_val < value;

      if (comp_op == ">")
        return attr_val > value;

      if (comp_op == "=")
        return attr_val == value;

      if (comp_op == "!=")
        return attr_val != value;
    }

    return false;
  }
};

#endif
