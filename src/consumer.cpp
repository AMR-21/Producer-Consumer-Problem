#include "headers/utility.h"
#include "headers/buffer.h"
#include "headers/conUtil.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <string>
using namespace std;

extern struct sembuf semWait;
extern struct sembuf semSignal;
extern key_t keyB, keyN;
extern int s, b, n;

int shBuf, shIt;
Commodity *c;
vector<Item *> items;

int main(int argc, char **argv)
{
  signal(SIGINT, exit_handler);

  if (argc < 2)
  {
    printf("Too few arguments");
    printf("\nAborting execution ...\n");
    exit(0);
  }

  // Create items
  createItems(&items);

  printDashboard(items);

  // Initialize sems/shms
  initialize();

  // Initialize buffer and items array
  initBuffer(stoi(argv[1]));

  // Initialize buffer semaphore
  signalBuffer(stoi(argv[1]));

  while (true)
  {
    semop(n, &semWait, 1);
    semop(s, &semWait, 1);

    // connect to buffer and items array
    if ((shBuf = shmget(keyB, 0, 0666)) < 0)
    {
      semop(s, &semSignal, 1);
      continue;
    };

    if ((shIt = shmget(keyN, 0, 0666)) < 0)
    {
      semop(s, &semSignal, 1);
      continue;
    };

    Buffer *buffer = (Buffer *)shmat(shBuf, 0, 0);
    buffer->items = (int *)shmat(shIt, 0, 0);

    // BEGIN CRITICAL SECTION
    int cid = buffer->deQueue();
    c = (Commodity *)shmat(cid, 0, 0);
    // END CRITICAL SECTION

    semop(s, &semSignal, 1);
    semop(b, &semSignal, 1);
    shmdt(buffer);

    consume(c->name, c->price, items);
  }

  return 1;
}