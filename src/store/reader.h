//lang::CwC
#pragma once

// author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
class Reader : public Object {
public:
  // visits the row
  virtual bool visit(Row& r) { 
    return false;
  }
};