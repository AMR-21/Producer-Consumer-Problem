#include "headers/conUtil.h"
#include <vector>
#include <iostream>
#include <string.h>
#include <boost/algorithm/string.hpp>

using namespace std;

Item::Item(string name)
{
  Item::name = name;
  Item::price = 0;
  Item::average = 0;
  Item::count = 0;
  Item::status = "";
}

void Item::calcAverage()
{

  // hold only 5 prices from newest to oldest
  if (history.size() > 5)
  {
    history.erase(history.begin());
  }

  double sum = 0;

  for (double p : history)
    sum += p;

  average = sum / (history.size() * 1.0);
}

void createItems(vector<Item *> *items)
{
  string commodities[11] = {
      "ALUMINIUM",
      "COPPER",
      "COTTON",
      "CRUDEOIL",
      "GOLD",
      "LEAD",
      "MENTHAOIL",
      "NATURALGAS",
      "NICKEL",
      "SILVER",
      "ZINC"};

  int i = 4;
  for (string com : commodities)
  {
    Item *t = new Item(com);
    t->col = i++;
    items->push_back(t);
  }
}

void printDashboard(vector<Item *> items)
{
  printf("\e[1;1H\e[2J");
  printf("+-------------------------------------+\n");
  printf("| Commodity     |  Price   | AvgPrice |\n");
  printf("+-------------------------------------+\n");

  for (Item *t : items)
  {
    string color;

    if (t->price == 0)
      color = "\033[0;34m"; // blue
    else if (t->status == "↑")
      color = "\033[0;32m"; // green
    else
      color = "\033[0;31m"; // red

    printf("| %-11s   |", t->name.c_str());
    printf("%s", color.c_str());
    printf(" %7.2lf", t->price);
    printf("%s ", t->status != "" ? t->status.c_str() : " ");
    printf("\033[0m"); // default
    printf("|");
    printf("%s", color.c_str());
    printf(" %7.2lf", t->average);
    printf("%s ", t->status != "" ? t->status.c_str() : " ");
    printf("\033[0m");
    printf("|\n");
  }

  printf("+-------------------------------------+\n");
  fflush(stdout);
}

void update(Item *t)
{
  string col = to_string(t->col);
  string pPos = "\033[" + col + ";18H";
  string aPos = "\033[" + col + ";29H";
  string color;
  if (t->status == "↑")
    color = "\033[0;32m"; // green
  else
    color = "\033[0;31m"; // red

  printf("%s", color.c_str());

  printf("%s", pPos.c_str());
  printf(" %7.2lf", t->price);
  printf("%s", t->status != "" ? t->status.c_str() : " ");
  printf("%s", aPos.c_str());
  printf(" %7.2lf", t->average);
  printf("%s", t->status != "" ? t->status.c_str() : " ");
  printf("\033[0m");

  aPos = "\033[" + col + ";40H";
  printf("%s", aPos.c_str());
  fflush(stdout);
}

void consume(string name, double price, vector<Item *> items)
{
  bool flag = false;
  Item *item;

  for (Item *t : items)
  {
    if (boost::iequals(t->name, name))
    {
      item = t;
      flag = true;
      break;
    }
  }

  if (!flag)
    return;

  price >= item->price ? item->status = "↑" : item->status = "↓";

  item->price = price;
  item->history.push_back(price);
  item->calcAverage();
  update(item);
}