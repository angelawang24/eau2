// lang::Cwc
#pragma once

#include "reader.h"
#include "../util/simap.h"

// author: instructors of cs4500
class Adder : public Reader {
public:
  SIMap& map_;  // String to int map;
 
  Adder(SIMap& map) : map_(map)  {}
 
  bool visit(Row& r) override {
    String* word = new String(r.get_string(0)->c_str());
    assert(word != nullptr);
    int num = map_.contains(*word) ? map_.get(*word) : 0;
    num++;
    map_.set(*word, num);
    delete word;
    return false;
  }
};