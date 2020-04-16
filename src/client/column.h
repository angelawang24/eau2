// Lang::CwC
#pragma once
#include "../util/object.h"
#include "../util/string.h"
#include "../util/array.h"
#include <stdarg.h>
#include <math.h>

size_t column_size = 1024;
class IntColumn;
class BoolColumn;
class DoubleColumn;
class StringColumn;

/**************************************************************************
 * Column ::
 * Represents one column of a data frame which holds values of a single type.
 * This abstract class defines methods overriden in subclasses. There is
 * one subclass per element type. Columns are mutable, equality is pointer
 * equality. */
// author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
class Column : public Object {
public:    
    size_t cap_; // the max capacity of this column for pointers to arrays
    size_t len_; // the current number of pointers to arrays in the column
    size_t num_elements_; // the number of elements in the column

    /** Type converters: Return same column under its actual type, or
     *  nullptr if of the wrong type.  */
    virtual IntColumn* as_int() { return nullptr; }
    virtual BoolColumn*  as_bool() { return nullptr; }
    virtual DoubleColumn* as_double() { return nullptr; }
    virtual StringColumn* as_string() { return nullptr; }

    /** Type appropriate push_back methods. Calling the wrong method is
     * undefined behavior. **/
    virtual void push_back(int val) { assert(false); }
    virtual void push_back(bool val) { assert(false); }
    virtual void push_back(double val) { assert(false); }
    virtual void push_back(String* val) { assert(false); }

    /** Returns the number of elements in the column. */
    size_t size() { return this->num_elements_; }

    /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'. */
    char get_type() { return this->get_type_helper_(); }

    /** Returns the type of the column */
    virtual char get_type_helper_() { return 'N'; }

    virtual bool equals(Object *const obj) { assert(false); }

    void check_size_(size_t idx) { assert(idx < this->size()); }
};
 
/*************************************************************************
 * IntColumn::
 * Holds primitive int values, unwrapped.
 */
class IntColumn : public Column {
public:
    IntArray** arr_; // represents the column data

    IntColumn() {
        this->arr_ = new IntArray*[column_size];
        this->arr_[0] = new IntArray();
        this->cap_ = column_size;
        this->len_ = 1; // number of arrays
        this->num_elements_ = 0; // number of elements in column
    }

    /** Copy constructor */
    // Used for adding column to a dataframe
    IntColumn(IntColumn& c) {
        this->arr_ = new IntArray*[column_size];
        this->arr_[0] = new IntArray();
        this->num_elements_ = 0;
        this->cap_ = column_size;
        this->len_ = 1;
        for (size_t i = 0; i < c.size(); i++) {
            this->push_back(c.get(i));
        }   
    }

    /** Destructor */
    ~IntColumn() {
        for (size_t i = 0; i < len_; i++) {
            delete this->arr_[i];
        }
        delete [] this->arr_;
    }

    // Allocates more space for the array by doubling when length is equal to capacity
    void allocate_() {
        if (this->len_ == this->cap_) {
            IntArray** temp = new IntArray*[2 * this->cap_];

            for (size_t i = 0; i < this->cap_; i++) {
                temp[i] = this->arr_[i];
            }
            delete[] this->arr_;
            this->arr_ = temp;
            this->cap_ *= 2;
        }
    }

    /** Returns element at provided index, error thrown if index is out of bounds 
     * @arg idx Index at which to get element
     */
    int get(size_t idx) {
        check_size_(idx);
        
        size_t locationInColumn = idx / column_size; // Get offset in column
        size_t locationInArray = idx % column_size; // Get offset in array in column

        IntArray* head = this->arr_[locationInColumn];    
        return head->get(locationInArray);
    }

    IntColumn* as_int() { return this; }

    // Used for deserializing a dataframe
    void push_back(int val) { 
        if (this->arr_[this->len_ - 1]->is_full()) {
            this->allocate_();
            this->arr_[this->len_] = new IntArray();
            this->arr_[this->len_]->add(val);
            this->len_++;
        } else {
            this->arr_[this->len_ - 1]->add(val);
        }
        this->num_elements_++;
        
    }
    char get_type_helper_() { return 'I'; }
};
 
 /*************************************************************************
 * BoolColumn::
 * Holds primitive bool values, unwrapped.
 */
class BoolColumn : public Column {
public:
    BoolArray** arr_;

    BoolColumn() {
        this->arr_ = new BoolArray*[column_size];
        this->arr_[0] = new BoolArray();
        this->cap_ = column_size;
        this->len_ = 1;
        this->num_elements_ = 0;
    }

    /** Copy constructor */
    BoolColumn(BoolColumn& c) {
        this->arr_ = new BoolArray*[column_size];
        this->arr_[0] = new BoolArray();
        this->num_elements_ = 0;
        this->cap_ = column_size;
        this->len_ = 1;
        for (size_t i = 0; i < c.size(); i++) {
            this->push_back(c.get(i));
        }   
    }

    /** Destructor */
    ~BoolColumn() {
        for (size_t i = 0; i < len_; i++) {
            delete this->arr_[i];
        }
        delete [] this->arr_;
    }

    // Allocates more space for the array by doubling when length is equal to capacity
    void allocate_() {
        if (this->len_ == this->cap_) {
            BoolArray** temp = new BoolArray*[2 * this->cap_];
            for (size_t i = 0; i < this->cap_; i++) {
                temp[i] = this->arr_[i];
            }
            delete[] this->arr_;
            this->arr_ = temp;
            this->cap_ *= 2;
        }
    }

    /** Returns element at provided index, error thrown if index is out of bounds 
     * @arg idx Index at which to get element
     */
    bool get(size_t idx) {
        check_size_(idx);
        size_t locationInColumn = idx / column_size; // Get offset in column
        size_t locationInArray = idx % column_size; // Get offset in array in column

        BoolArray* head = this->arr_[locationInColumn];    
        return head->get(locationInArray);
    }

    BoolColumn* as_bool() { return this; }

    void push_back(bool val) {
        if (this->arr_[this->len_ - 1]->is_full()) {
            this->allocate_();
            this->arr_[this->len_] = new BoolArray();
            this->arr_[this->len_]->add(val);
            this->len_++;
        } else {
            this->arr_[this->len_ - 1]->add(val);
        }
        this->num_elements_++; 
    }
    char get_type_helper_() { return 'B'; }
};
 
 /*************************************************************************
 * DoubleColumn::
 * Holds primitive Double values, unwrapped.
 */
class DoubleColumn : public Column {
public:
    DoubleArray** arr_;

    DoubleColumn() {
        this->arr_ = new DoubleArray*[column_size];
        this->arr_[0] = new DoubleArray();
        this->cap_ = column_size;
        this->len_ = 1;
        this->num_elements_ = 0;
    }

    /** Copy constructor */
    DoubleColumn(DoubleColumn& c) {
        this->arr_ = new DoubleArray*[column_size];
        this->arr_[0] = new DoubleArray();
        this->num_elements_ = 0;
        this->cap_ = column_size;
        this->len_ = 1;
        for (size_t i = 0; i < c.size(); i++) {
            this->push_back(c.get(i));
        }   
    }

    /** Destructor */
    ~DoubleColumn() {
        for (size_t i = 0; i < len_; i++) {
            delete this->arr_[i];
        }
        delete [] this->arr_;
    }

    // Allocates more space for the array by doubling when length is equal to capacity
    void allocate_() {
        if (this->len_ == this->cap_) {
            DoubleArray** temp = new DoubleArray*[2 * this->cap_];
            for (size_t i = 0; i < this->cap_; i++) {
                temp[i] = this->arr_[i];
            }
            delete[] this->arr_;
            this->arr_ = temp;
            this->cap_ *= 2;
        }
    }

    /** Returns element at provided index, error thrown if index is out of bounds 
     * @arg idx Index at which to get element
     */
    double get(size_t idx) {
        check_size_(idx);
        size_t locationInColumn = idx / column_size; // Get offset in column
        size_t locationInArray = idx % column_size; // Get offset in array in column

        DoubleArray* head = this->arr_[locationInColumn];    
        return head->get(locationInArray);
    }
    DoubleColumn* as_double() {
        return this;
    }

    void push_back(double val) {
        if (this->arr_[this->len_ - 1]->is_full()) {
            this->allocate_();
            this->arr_[this->len_] = new DoubleArray();
            this->arr_[this->len_]->add(val);
            this->len_++;
        } else {
            this->arr_[this->len_ - 1]->add(val);
        }
        this->num_elements_++;   
    }
    char get_type_helper_() { return 'F'; }
};
 

/*************************************************************************
 * StringColumn::
 * Holds string pointers. The strings are external.  Nullptr is a valid
 * value.
 */
class StringColumn : public Column {
public:
    StringArray** arr_;

    StringColumn() {
        this->arr_ = new StringArray*[column_size];
        this->arr_[0] = new StringArray();
        this->cap_ = column_size;
        this->len_ = 1;
        this->num_elements_ = 0;
    }

    /** Copy constructor */
    StringColumn(StringColumn& c) {
        this->arr_ = new StringArray*[column_size];
        this->arr_[0] = new StringArray();
        this->num_elements_ = 0;
        this->cap_ = column_size;
        this->len_ = 1;
        for (size_t i = 0; i < c.size(); i++) {
            this->push_back(new String(c.get(i)->c_str()));
        }   
    }

    /** Destructor */
    ~StringColumn() {
        for (size_t i = 0; i < len_; i++) {
            delete this->arr_[i];
        }
        delete [] this->arr_;
    }

    // Allocates more space for the array by doubling when length is equal to capacity
    void allocate_() {
        if (this->len_ == this->cap_) {
            StringArray** temp = new StringArray*[2 * this->cap_];

            for (size_t i = 0; i < this->cap_; i++) {
                temp[i] = this->arr_[i];
            }
            delete[] this->arr_;
            this->arr_ = temp;
            this->cap_ *= 2;
        }
    }

    /** Returns element at provided index, error thrown if index is out of bounds 
     * @arg idx Index at which to get element
     */
    String* get(size_t idx) {
        check_size_(idx);
        size_t locationInColumn = idx / column_size; // Get offset in column
        size_t locationInArray = idx % column_size; // Get offset in array in column

        StringArray* head = this->arr_[locationInColumn];    
        return head->get(locationInArray);
    }
    StringColumn* as_string() { return this; }

    void push_back(String* val) {
        if (this->arr_[this->len_ - 1]->is_full()) {
            this->allocate_();
            this->arr_[this->len_] = new StringArray();
            this->arr_[this->len_]->add(val);
            this->len_++;
        } else {
            this->arr_[this->len_ - 1]->add(val);
        }
        this->num_elements_++; 
    }
    char get_type_helper_() { return 'S'; }
};