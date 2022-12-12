#ifndef _CONUTIL_H
#define _CONUTIL_H
#include <vector>
#include <string>
using namespace std;

struct Item
{
  string name;
  string status;
  double price;
  double average;
  int count;
  int col;
  vector<double> history;

  Item(string name);
  void calcAverage();
};

void createItems(vector<Item *> *items);

void printDashboard(vector<Item *> items);

void update(Item *t);

void consume(string name, double price, vector<Item *> items);
#endif