//lang::CwC
#pragma once
#include "../store/kdstore.h"

// authors: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
// Application Class represents an Application that can be defined. 
// These applications can be used to perform functions on data in dataframes 
// stored in a KeyValueStore
class Application : public Object {
public:
    KeyValueStore* kv;
    KDStore* kd;
    size_t index;

    // Constructor
    Application(size_t idx) {
        index = idx;
        kv = new KeyValueStore(idx);
        kd = new KDStore(kv);
    }

    // Destructor
    ~Application() { delete kd; }

    // Returns the node index that the application is running on
    size_t this_node() { return index; }

    // Runs the application
    virtual void run_() {}

    // Runs the application
    virtual void run_(size_t thread_node) {}
};