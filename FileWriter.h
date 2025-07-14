#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <any>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
#include "Record.h"

class FileWriter {
private:
  ofstream fout;

public:
  void open(string fileName) { fout.open(fileName); }

  void writePage(vector<Record> page, bool first_write) {

    if (first_write) {
      // Writing the names/ attribute names
      for (int i = 0; i < page[0].get_attrNames().size(); i++) {
        fout << page[0].get_attrNames()[i];

        if (i != page[0].get_attrNames().size() - 1) {
          fout << ",";
        }
      }
      fout << endl;

      // Writing the type/ attribute types
      for (int i = 0; i < page[0].get_attrTypes().size(); i++) {
        fout << page[0].get_attrTypes()[i];

        if (i != page[0].get_attrTypes().size() - 1) {
          fout << ",";
        }
      }
      fout << endl;

      // Write the junk line
      string junkLine = "----------------------";
      fout << junkLine << endl;
    }

    // Write the page values
    for (Record rec : page) {
      for (int i = 0; i < rec.attributes.size(); i++) {
        auto attr = rec.attributes[i];
        if (attr.type == "Double") {
          fout << any_cast<double>(attr.value);
        } else if (attr.type == "Integer") {
          fout << any_cast<int>(attr.value);
        } else {
          fout << any_cast<string>(attr.value);
        }

        if (i != rec.attributes.size() - 1) {
          fout << ",";
        }
      }
      fout << endl;
    }
  }

  void close() {
    if (fout.is_open())
      fout.close();
  }
};

#endif
