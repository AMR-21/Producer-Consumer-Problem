#include "headers/utility.h"
#include "headers/buffer.h"
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>
#include <random>
#include <chrono>
#include <cstdlib>
#include <string.h>
using namespace std;

struct sembuf semWait;
struct sembuf semSignal;
key_t key, keyF, keyB, keyN;
int shBuffer, shItems, semFlag, s, b, n;

void Commodity::init(char *name, double mean, double deviation, int sleep)
{
  // Commodity::name = (char *)calloc(sizeof(char *), 11);
  strcpy(Commodity::name, name);
  Commodity::name[10] = 0;
  Commodity::mean = mean;
  Commodity::deviation = deviation;
  Commodity::sleep = sleep;
}

void Commodity::generatePrice()
{
  unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
  default_random_engine e(seed);

  normal_distribution<double> distN(mean, deviation);
  price = abs(distN(e));
}

void initialize()
{

  semWait = {0, -1, SEM_UNDO};
  semSignal = {0, 1, SEM_UNDO};

  // ftok to generate unique key
  key = ftok("/", 65);  // key for flag shm / p/c sem
  keyB = ftok("/", 75); // key for buffer sem/shm
  keyN = ftok("/", 85); // key for commodities sem/shm
  keyF = ftok("/", 95); // key for commodities sem/shm

  // Initialize produce/consume semaphore
  if ((semFlag = shmget(key, 1, 0666 | IPC_CREAT)) == -1)
  {
    perror("Shared memory: ");
    exit(0);
  };

  bool *flag = (bool *)shmat(semFlag, NULL, 0);

  if ((s = semget(key, 1, 0666 | IPC_CREAT)) == -1)
  {
    perror("Semaphore: ");
    exit(0);
  };

  // Check if semaphore is initialized
  if (!*flag)
  {
    if (semctl(s, 0, SETVAL, 1) < 0)
    {
      perror("error");
      exit(0);
    }
    *flag = true;
  }

  // Initialize produce/consume semaphore
  if ((semFlag = shmget(keyF, 1, 0666 | IPC_CREAT)) == -1)
  {
    perror("Shared memory: ");
    exit(0);
  };

  bool *flage = (bool *)shmat(semFlag, NULL, 0);

  // Initialize buffer semaphore
  if ((b = semget(keyB, 1, 0666 | IPC_CREAT)) == -1)
  {
    perror("Semaphore: ");
    exit(0);
  };

  // Check if semaphore is initialized
  if (!*flage)
  {
    if (semctl(b, 0, SETVAL, 0) < 0)
    {
      perror("error");
      exit(0);
    }
    *flage = true;
  }

  // Initialize commodities semaphore
  if ((n = semget(keyN, 1, 0666 | IPC_CREAT)) == -1)
  {
    perror("Semaphore: ");
    exit(0);
  };

  if (semctl(n, 0, SETVAL, 0) < 0)
  {
    perror("error");
    exit(0);
  }
}

void initBuffer(int n)
{

  // Initialize shared buffer
  if ((shBuffer = shmget(keyB, sizeof(sizeof(Buffer *) * n), 0666 | IPC_CREAT)) == -1)
  {
    perror("Shared memory ");
    exit(0);
  };

  if ((shItems = shmget(keyN, sizeof(sizeof(int) * n), 0666 | IPC_CREAT)) == -1)
  {
    perror("Shared memory ");
    exit(0);
  };

  // initialize buffer with given parameters
  Buffer *buffer = (Buffer *)shmat(shBuffer, 0, 0);
  buffer->init(n, shItems);
}

void signalBuffer(int n)
{
  if (semctl(b, 0, SETVAL, n) < 0)
  {
    perror("error");
    exit(0);
  }
}

void exit_handler(int signo)
{

  // clear shared memories
  shmctl(shBuffer, IPC_RMID, NULL);
  shmctl(shItems, IPC_RMID, NULL);

  // Initialize produce/consume semaphore
  if ((semFlag = shmget(keyF, 1, 0666)) == -1)
  {
    perror("Shared memory: ");
    exit(0);
  };

  bool *flage = (bool *)shmat(semFlag, NULL, 0);

  if ((b = semget(keyB, 1, 0666)) == -1)
  {
    perror("Semaphore: ");
    exit(0);
  };

  if (semctl(b, 0, SETVAL, 0) < 0)
  {
    perror("error");
    exit(0);
  }
  *flage = false;

  printf("\033[2D  ");
  printf("\033[17;1H");

  exit(0);
};

// void clear()
// {
//   semctl(s, 0, IPC_RMID);
//   semctl(e, 0, IPC_RMID);
// }

// void clear(std::queue<int> &q)
// {
//   std::queue<int> empty;
//   std::swap(q, empty);
// }
