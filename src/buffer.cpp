#include "headers/buffer.h"
#include "headers/utility.h"

void Buffer::init(int n, int shItems)
{

  Buffer::front = -1;
  Buffer::rear = -1;
  Buffer::size = n;
  Buffer::items = (int *)shmat(shItems, 0, 0);
}

bool Buffer::isFull()
{
  if (front == 0 && rear == size - 1)
  {
    return true;
  }
  if (front == rear + 1)
  {
    return true;
  }
  return false;
}

bool Buffer::isEmpty()
{
  if (front == -1)
    return true;
  else
    return false;
}

void Buffer::enQueue(int c)
{
  if (!isFull())
  {
    if (front == -1)
      front = 0;
    rear = (rear + 1) % size;
    items[rear] = c;
  }
}

int Buffer::deQueue()
{
  int c;
  if (!isEmpty())

  {
    c = items[front];
    if (front == rear)
    {
      front = -1;
      rear = -1;
    }

    else
    {
      front = (front + 1) % size;
    }
    return c;
  }
  return -1;
}
