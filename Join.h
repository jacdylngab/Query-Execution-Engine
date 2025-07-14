#ifndef JOIN_H
#define JOIN_H

#include "FileReader.h"
#include "FileWriter.h"
#include "Operator.h"
#include <any>
#include <string>
#include <unistd.h>
#include <vector>

class Join : public Operator {
private:
  Operator *leftInput;
  Operator *rightInput;
  FileReader fr1;
  FileReader fr2;
  FileWriter fw;
  string pAttr1;
  string pAttr2;
  vector<Record> leftPage;
  vector<Record> rightPage;
  int leftIndex = 0;
  int rightIndex = 0;
  bool first_pass_through = true;
  bool materialization = false;
  static inline int tempFileCount;
  string tempFileName;
  int leftPageCount = 0;

public:
  Join(Operator *op1, Operator *op2, string pAttr1, string pAttr2)
      : leftInput(op1), rightInput(op2), pAttr1(pAttr1), pAttr2(pAttr2) {
    materialization = true;
    tempFileName = "tempFile" + to_string(++tempFileCount) + ".txt";
  }
  Join(Operator *op, string fileName, string pAttr1, string pAttr2)
      : leftInput(op), fr1(fileName), rightInput(&fr1), pAttr1(pAttr1),
        pAttr2(pAttr2) {}
  Join(string fileName1, string fileName2, string pAttr1, string pAttr2)
      : fr1(fileName1), leftInput(&fr1), fr2(fileName2), rightInput(&fr2),
        pAttr1(pAttr1), pAttr2(pAttr2) {}

  void open() {
    leftInput->open();
    rightInput->open();
  }

  vector<Record> next() {
    vector<Record> outPage;

    if (first_pass_through) {
      leftPage = leftInput->next();
      rightPage = rightInput->next();
      first_pass_through = false;

      if (materialization) {
        fw.open(tempFileName);
        fw.writePage(rightPage, true);
      }
    }

    // While outPage is not full
    while (outPage.size() < pageSize) {
      // Advance iterators to next pairing of records
      advance_iterators();

      // return early if no more data
      if (leftPage.empty() || rightPage.empty()) {
        break;
      }

      // cout << "left: " << leftIndex << " right:" << rightIndex << endl;
      Record leftInRecord = leftPage[leftIndex];
      Record rightInRecord = rightPage[rightIndex];

      // Get the left and right attributes
      any left_attr = leftInRecord.getAttribute(pAttr1).value;
      any right_attr = rightInRecord.getAttribute(pAttr2).value;

      // If record attributes match
      if (compare_attributes(left_attr, right_attr)) {
        // Combine leftInRecord and rightInRecord into one
        leftInRecord.attributes.insert(leftInRecord.attributes.end(),
                                       rightInRecord.attributes.begin(),
                                       rightInRecord.attributes.end());

        // add leftRec + righRec to outPage
        outPage.push_back(leftInRecord);
      }
    }
    return outPage;
  }

  void close() {
    leftInput->close();
    rightInput->close();
  }

  // Function to advance the iterators to next pairing of records
  void advance_iterators() {
    // The right index changes the most so we should increment it first
    rightIndex++;

    if (rightIndex == rightPage.size()) {
      rightIndex = 0;
      leftIndex++;

      if (leftIndex == leftPage.size()) {
        leftIndex = 0;
        rightPage = rightInput->next();

        if (!rightPage.empty() && materialization) {
          fw.writePage(rightPage, false);
        }

        if (rightPage.empty()) {
          leftPage = leftInput->next();
          if (leftPage.empty()) {
            return; // Stop processing both pages are empty
          }
          rightInput->close();
          if (materialization) {
            materialization = false;
            fw.close();
            fr2.setFileName(tempFileName);
            rightInput = &fr2;
          }
          rightInput->open();
          rightPage = rightInput->next();

          if (rightPage.empty())
            cout << "BAD: Just close/open right side and got back empty page"
                 << endl;
        }
      }
    }
  }

  // Function to validate the type
  bool compare_attributes(any left_attr, any right_attr) {
    // Check for type compatibility
    if (left_attr.type() != right_attr.type()) {
      return false;
    }

    // compare based on type
    if (left_attr.type() == typeid(int)) {

      return any_cast<int>(left_attr) == any_cast<int>(right_attr);
    } else if (left_attr.type() == typeid(double)) {
      return any_cast<double>(left_attr) == any_cast<double>(right_attr);
    } else if (left_attr.type() == typeid(string)) {
      return any_cast<string>(left_attr) == any_cast<string>(right_attr);
    }

    return false; // Unsupported type
  }
};

#endif
