#include "headers/utility.h"
#include "headers/buffer.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <algorithm>
#include <fcntl.h>
using namespace std;

extern struct sembuf semWait;
extern struct sembuf semSignal;
extern key_t keyB, keyN;
extern int s, b, n;

int shBuf, shIt;
Commodity *c;

string getTime()
{
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  long tn = ts.tv_nsec;
  string nRes = to_string(tn / 1000000000.0);
  nRes = nRes.substr(1, 4);

  tm *t = gmtime(&ts.tv_sec);
  string mins = to_string(t->tm_min);
  if (mins.length() < 2)
    mins.insert(0, "0");

  string hrs = to_string(t->tm_hour);
  if (hrs.length() < 2)
    hrs.insert(0, "0");

  string scs = to_string(t->tm_sec);
  if (scs.length() < 2)
    scs.insert(0, "0");

  string mon = to_string(t->tm_mon);
  if (mon.length() < 2)
    mon.insert(0, "0");

  string day = to_string(t->tm_mday);
  if (day.length() < 2)
    day.insert(0, "0");

  return "[" +
         mon + "/" + day + "/" + to_string(1900 + t->tm_year) + " " + hrs + ":" + mins + ":" + scs + nRes + "]";
}

string uppercase(char *name)
{
  string s1 = name;
  transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
  return s1;
}

void logMessage(int id, char *name, double price, int sleep)
{
  // string fName = string(name) + ".txt";
  // int defaulterr = dup(2);
  // int logFl = open(fName.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666);
  // dup2(logFl, 2);

  string timestamp = getTime();
  switch (id)
  {
  case 1:
    fprintf(stderr, "%s %s : generating a new value %.2f\n", timestamp.c_str(), uppercase(name).c_str(), price);
    break;
  case 2:
    fprintf(stderr, "%s %s : trying to get mutex on shared buffer\n", timestamp.c_str(), uppercase(name).c_str());
    break;
  case 3:
    fprintf(stderr, "%s %s : placing %.2f on shared buffer\n", timestamp.c_str(), uppercase(name).c_str(), price);
    break;
  case 4:
    fprintf(stderr, "%s %s : sleeping for %d ms\n\n", timestamp.c_str(), uppercase(name).c_str(), sleep);
    break;
  }

  // close(logFl);
  // dup2(defaulterr, 2);
}

int main(int argc, char **argv)
{

  if (argc < 6)
  {
    printf("%s : Too few arguments", argv[1]);
    printf("\nAborting execution ...\n");
    exit(0);
  }

  int cid;

  // Init commodity in shared memory
  int r = rand();
  key_t keyC = abs(r) + argv[1][0] + argv[1][1] + argv[1][2] + stoi(argv[2]) + stoi(argv[3]) + stoi(argv[4]);

  if ((cid = shmget(keyC, sizeof(Commodity *), 0666 | IPC_CREAT)) == -1)
  {
    perror("Inside producer commodity shared memory ");
    exit(0);
  };

  c = (Commodity *)shmat(cid, 0, 0);
  c->init(argv[1], stod(argv[2]), stod(argv[3]), stoi(argv[4]));

  // Initialize sems/shms
  initialize();

  // Initialize buffer and items array
  initBuffer(stoi(argv[5]));

  while (true)
  {
    c = (Commodity *)shmat(cid, 0, 0);
    c->generatePrice(); // produce function
    logMessage(1, c->name, c->price, c->sleep);

    logMessage(2, c->name, c->price, c->sleep);
    semop(b, &semWait, 1);
    semop(s, &semWait, 1);
    logMessage(3, c->name, c->price, c->sleep);

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
    buffer->enQueue(cid);
    // END CRITICAL SECTION

    semop(s, &semSignal, 1);
    semop(n, &semSignal, 1);
    shmdt(buffer);
    logMessage(4, c->name, c->price, c->sleep);
    usleep(c->sleep * 1000);
  }

  return 1;
}