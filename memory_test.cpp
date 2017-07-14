#include <time.h>
#include <iostream>

using namespace std;

class Data {
 public:
  Data(int d): data_(d) {}

  int data() { return data_; }

 private:
  int data_;
};

class DataWithPool {
 public:
  DataWithPool(int d): data_(d) {}

  int data() { return data_; }

  static void *operator new(size_t size) {
    DataWithPool *ptr;
    if (!freeStore) {
      size_t chunk = BLOCKS * size;
      ptr = reinterpret_cast<DataWithPool*>(new char[chunk]);
      freeStore = ptr;
      for (; ptr != &freeStore[BLOCKS - 1]; ++ptr) {
        ptr->next = ptr + 1;
      }
      ptr->next = 0;
    }
    ptr = freeStore;
    freeStore = freeStore->next;
    return ptr;
  }

  static void operator delete(void *ptr, size_t size) {
    (static_cast<DataWithPool*>(ptr))->next = freeStore;
    freeStore = static_cast<DataWithPool*>(ptr);
  }

 private:
  static DataWithPool *freeStore;
  static const int BLOCKS = 10000;

  DataWithPool *next;
  int data_;
};

DataWithPool *DataWithPool::freeStore = 0;

int main() {
  cout << "Data:\n";
  clock_t start, finish;
  start = clock();
  Data *d[10000];

  for (int i = 0; i < 10000; ++i) {
    d[i] = new Data(i);
  }

  for (int i = 0; i < 10; ++i) {
    cout << d[i] << " " << d[i]->data() << endl;
  }

  for (int i = 0; i < 10000; ++i) {
    delete d[i];
  }

  finish = clock();
  cout << "duration: " << (double)(finish - start) / CLOCKS_PER_SEC << endl;

  cout << "DataWithPool:\n";

  start = clock();
  DataWithPool *dp[10000];

  for (int i = 0; i < 10000; ++i) {
    dp[i] = new DataWithPool(i);
  }

  for (int i = 0; i < 10; ++i) {
    cout << dp[i] << " " << dp[i]->data() << endl;
  }

  for (int i = 0; i < 10000; ++i) {
    delete dp[i];
  }

  finish = clock();
  cout << "duration: " << (double)(finish - start) / CLOCKS_PER_SEC << endl;
}