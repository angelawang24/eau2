// Lang::CwC
#pragma once

#include "object.h"
// author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu

/**
 * Represents an Integer data type. Needs to be an Object because row stores
 * the elements as an ObjectArray where every element is an Object.
 */
class Integer : public Object
{
public:
    int val_ = 0;

    // Constructor that takes in the primitive data, used in Row
    Integer(int val)
    {
        val_ = val;
    }
    // Destructor
    ~Integer() {}

    int get()
    {
        return val_;
    }

    void set(int val)
    {
        val_ = val;
    }

    /** Hash function overridden from Object class */
    size_t hash() {
        return val_;
    }

};

/**
 * Represents an Boolean data type.
 */
class Boolean : public Object
{
public:
    bool val_ = 0;

    // Constructor that takes in the primitive data
    Boolean(bool val)
    {
        val_ = val;
    }

    // Destructor
    ~Boolean() {}

    bool get()
    {
        return val_;
    }

    void set(bool val)
    {
        val_ = val;
    }

    /** Hash function overridden from Object class */
    size_t hash() {
        return val_;
    }
};

/**
 * Represents a Double data type.
 */
class Double : public Object
{
public:
    double val_ = 0;

    // Constructor that takes in the primitive data
    Double(double val)
    {
        val_ = val;
    }

    // Destructor
    ~Double() {}

    double get()
    {
        return val_;
    }

    void set(double val)
    {
        val_ = val;
    }

    /** Hash function overridden from Object class */
    size_t hash() {
        return static_cast<size_t>(val_*100000.0);
    }
};
