#include <thread>

#include "../src/application/demo.h"

// to run milestone 3
// authors: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu

int main() {
  std::thread* spawned_threads[3];
  Demo* d = new Demo(0);
  for (size_t i = 0; i < 3; i++) {
      spawned_threads[i] = new std::thread(&Demo::run_, d, i);
  }

  for (size_t i = 0; i < 3; i++) {
      spawned_threads[i]->join();
  }
  delete d;
  for (size_t i = 0; i < 3; i++) {
    delete spawned_threads[i];
  }
}