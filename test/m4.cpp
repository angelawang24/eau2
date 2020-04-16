#include <stdio.h>
#include <thread>
#include "../src/application/wordcount.h"

// authors: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
int main(int argc, char** argv) {
  assert(argc == 2);

  std::thread* spawned_threads[3];
  WordCount* d = new WordCount(0, argv[1]);
  for (size_t i = 0; i < 3; i++) {
      spawned_threads[i] = new std::thread(&WordCount::run_, d, i);
  }

  for (size_t i = 0; i < 3; i++) {
      spawned_threads[i]->join();
  }
  delete d;
  for (size_t i = 0; i < 3; i++) {
      delete spawned_threads[i];
  }
}