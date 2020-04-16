#include "../client/dataframe.h"
#include "../client/application.h"
#include "../store/kdstore.h"

// Demo class
// authors: instructors of CS4500 with minor modifications from wang.ang@husky.neu.edu, mattie.r@husky.neu.edu

class Demo : public Application {
public:
  Key main{"main", 0};
  Key verify{"verif", 0};
  Key check{"ck", 0};
 
  Demo(size_t idx): Application(idx) { }
 
  void run_(size_t thread_node) override {
    switch(thread_node) {
    case 0:   producer();     break;
    case 1:   counter();      break;
    case 2:   summarizer();
   }
  }
 
  void producer() {
    size_t SZ = 100 * 1000;
    double* vals = new double[SZ];
    double sum = 0;
    for (size_t i = 0; i < SZ; ++i) {
        sum += vals[i] = i;
    }
    
    delete DataFrame::fromArray(&main, kv, SZ, vals);
    delete DataFrame::fromScalar(&check, kv, sum);
    delete [] vals;
  }
 
  void counter() {
    DataFrame* v = kd->waitAndGet(main);
    double sum = 0;
    for (size_t i = 0; i < 100 * 1000; ++i) sum += v->get_double(0,i);
    p("The sum is  ").pln(sum);
    delete v;
    delete DataFrame::fromScalar(&verify, kv, sum);
  }
 
  void summarizer() {
    DataFrame* result = kd->waitAndGet(verify);
    DataFrame* expected = kd->waitAndGet(check);
    pln(expected->get_double(0,0)==result->get_double(0,0) ? "SUCCESS":"FAILURE");
    delete result; delete expected;
  }
};