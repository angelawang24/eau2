//lang::c++
#pragma once

#include "../util/serial.h"
#include "keyvaluestore.h"

#include <mutex>
#include <unistd.h>
using namespace std;

mutex waitNGet;
// authors: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
// KDStore class represents a layer of abstraction between a DataFrame and a KeyValueStore. 
// It contains a KeyValueStore, and it supports the operation get on it. 
// It deserializes the serialized blob that KeyValueStore's get method returns and it returns a DataFrame.

class KDStore {
public:
    KeyValueStore* mykv;

    // Constructor
    KDStore(KeyValueStore *kv) {
        mykv = kv; 
    }

    // Destructor
    ~KDStore() {
        delete mykv;
    }

    // Returns dataframe deserialized from the KeyValueStore associated with the key passed in
    DataFrame* get(Key k) {
        Object* value = Serial::deserialize(mykv->get(k));
        DataFrame* df = dynamic_cast<DataFrame*>(value);

        assert(df != nullptr);
        return df;
    }

    // Returns serialized blob the corresponds to given key
    DataFrame* waitAndGet(Key k) {
        waitNGet.lock();
        Object* value = Serial::deserialize(mykv->waitAndGet(k));

        DataFrame* df = dynamic_cast<DataFrame*>(value);
        assert(df != nullptr);
        waitNGet.unlock();
        return df;
    }
};