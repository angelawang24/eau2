// lang::Cwc
#pragma once
#include "writer.h"

// author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
class Summer : public Writer {
public:
  SIMap& map_;
  std::unordered_map<string, int>::iterator it_;
  size_t i = 0;
  size_t j = 0;
  size_t seen = 0;
 
 //constructor
  Summer(SIMap& map) : map_(map) {
       it_ = map_.umap.begin();
  }
 
  // Visits row passed in and adds the data from the map to the row
  void visit(Row& r) {
    string k = it_->first;
    int value = it_->second;

    String* key = new String(k.c_str());

    r.set(0, key);
    r.set(1, (int)value);
    it_++;
    delete key;
  }
 
  // Returns whether or not the writer is finished
  bool done() {return it_ == map_.umap.end(); }
};