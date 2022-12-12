#ifndef _UTILITY_H
#define _UTILITY_H
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

using namespace std;

struct Commodity
{
  char name[11];
  double mean;
  double deviation;
  int sleep;
  double price;
  int nid;
  void generatePrice();
  void init(char *name, double mean, double deviation, int sleep);
};

void initialize();

void initBuffer(int n);

void exit_handler(int signo);

void signalBuffer(int n);
#endif