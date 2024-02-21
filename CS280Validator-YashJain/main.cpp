/* Programming Assignment 3
 *
 * main.cpp
 *
 * CS280 - Fall 2022
 * parser and interpreter testing program
 */

#include <fstream>
#include <iostream>

#include "lex.h"
#include "parseInt.h"
#include "val.h"

using namespace std;
// extern int error_count;

int main(int argc, char *argv[]) {
  int lineNumber = 1;

  istream *in = NULL;
  ifstream file;

  file.open("test.txt");
  in = &file;

  bool status = Prog(*in, lineNumber);

  if (!status) {
    cout << "\nUnsuccessful Interpretation " << endl
         << "Number of Errors " << ErrCount() << endl;
  } else {
    cout << "\nSuccessful Execution" << endl;
  }
}
