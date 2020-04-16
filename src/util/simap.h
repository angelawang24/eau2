//lang::CwC
#pragma once
#include "object.h"

//authors: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
// Represents a string to int map
class SIMap : public Object {
public:
  unordered_map<string, int> umap;
  SIMap() {}

  // Returns the value associated with the key
  int get(String key) {
    string k = string(key.c_str());
    assert(umap.find(k) != umap.end());
    return umap.find(k)->second; // first=key, second=value
  }

  /** Add item->val_ at item->key_ either by updating an existing pair or
    * creating a new one if not found.  */
  void set(String key, int v) {
    string k = string(key.c_str());
    umap[k] = v;
  }

  // Returns whether or not this map contains the given key
  bool contains(String key) {
    string k = string(key.c_str());
    return umap.find(k) != umap.end();
  }

  // Returns the size of the map
  size_t size() { return umap.size();}
};