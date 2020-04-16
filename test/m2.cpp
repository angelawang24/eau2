#include "../src/client/dataframe.h"
#include "../src/client/application.h"

/** Authors: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu */
class Trivial : public Application {
 public:
  Trivial(size_t idx) : Application(idx) { }
  void run_() {
    size_t SZ = 1000 * 1000;
    double* vals = new double[SZ];
    double sum = 0;
    for (size_t i = 0; i < SZ; ++i) {
        sum += vals[i] = i;
    }
    Key key("triv",0); 
    DataFrame* df = DataFrame::fromArray(&key, kv, SZ, vals);
    assert(df->get_double(0,1) == 1);
    DataFrame* df2 = kd->get(key);
    for (size_t i = 0; i < SZ; ++i) {
        sum -= df2->get_double(0,i);
    }
    assert(sum==0);
    delete df; delete df2; delete [] vals;
    OK("OK: trivial");
  }
};

void FAIL() { exit(1); }
void OK(const char *m)
{
    Sys *c = new Sys();
	c->p("OK: ");
	c->pln(m);
    delete c;
}
void t_true(bool p)
{
    if (!p)
        FAIL();
}
void t_false(bool p)
{
    if (p)
        FAIL();
}

int main() {
    Trivial(0).run_();
}