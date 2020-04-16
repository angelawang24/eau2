#pragma once
#include "../client/dataframe.h"
#include "../client/application.h"
#include "../store/filereader.h"
#include "../store/adder.h"
#include "../store/summer.h"
#include "../util/simap.h"
/****************************************************************************
 * Calculate a word count for given file:
 *   1) read the data (single node)
 *   2) produce word counts per homed chunks, in parallel
 *   3) combine the results
 **********************************************************author: pmaj ****/
class WordCount: public Application {
public:
  static const size_t BUFSIZE = 1024;
  Key in;
  KeyBuff kbuf;
  SIMap all;
  // size_t index;
  char* filename;
 
  WordCount(size_t idx, char* fname):
    Application(idx), in("data", 0), kbuf(new Key("wc-map-",0)) { 
      filename = fname;
    }

  /** The master nodes reads the input, then all of the nodes count. 
   * This run method specifies which node is running it */
  void run_(size_t thread_node) override {
    if (thread_node == 0) {
      FileReader fr(filename);
      delete DataFrame::fromVisitor(&in, kv, const_cast<char*>("S"), fr);
      pln("Read file");
    }
    local_count(thread_node);
    reduce(thread_node);
  }
 
  /** Returns a key for given node.  These keys are homed on master node
   *  which then joins them one by one. */
  Key* mk_key(size_t idx) {
      Key* kb = kbuf.get();
      Key * k = new Key(kb->key + to_string(idx), 0);
      char* kchar = k->c_str();
      p("Created key ");
      pln(kchar);
      delete kb; delete [] kchar;
      return k;
  }
 
  /** Compute word counts on the local node and build a data frame. */
  void local_count(size_t index) {
    DataFrame* words = kd->waitAndGet(in);
    p("Node ").p(index).pln(": starting local count...");
    SIMap map;
    Adder add(map);
    words->local_map(add, index);
    delete words;
    Summer cnt(map);
    Key* mk = mk_key(index);
    delete DataFrame::fromVisitor(mk, kv, const_cast<char*>("SI"), cnt);
    delete mk;
  }
 
  /** Merge the data frames of all nodes */
  void reduce(size_t index) {
    if (index != 0) return;
    pln("Node 0: reducing counts...");
    SIMap map;
    Key* own = mk_key(0);
    merge(kd->get(*own), map);
    pln("Ok merge");
    for (size_t i = 1; i < 3; ++i) { // merge other nodes
      Key* ok = mk_key(i);
      merge(kd->waitAndGet(*ok), map);
      delete ok;
    }
    
    p("Different words: ").pln(map.size());
    delete own;
  }
 
  void merge(DataFrame* df, SIMap& m) {
    Adder add(m);
    df->map(add);
    delete df;
  }
}; // WordcountDemo