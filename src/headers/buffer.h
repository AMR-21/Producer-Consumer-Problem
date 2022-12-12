#ifndef _BUFFER_H
#define _BUFFER_h

#include "utility.h"

struct Buffer
{
  int *items;
  int front;
  int rear;
  int size;

  bool isFull();
  bool isEmpty();

  void enQueue(int c);
  int deQueue();
  void init(int n, int shItems);
};

#endif