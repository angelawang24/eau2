//lang::CwC
#pragma once

#include "object.h"
#include "string.h"
#include <math.h>
using namespace std;
size_t cap_size = 1024;

string comma(",");
string semicolon(";");

double EPSILON = 0.0001;

/**
 * An basic Array class that should be inherited, but not directly used
 * author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu */
class Array : public Object
{
public:
    size_t cap_; // the max capacity of this array
    size_t len_; // the current length of the array

    /** CONSTRUCTORS & DESTRUCTORS **/

    /* Creates an Array of desired size */
    Array() {
        this->cap_ = cap_size;
        this->len_ = 0;
    }

    /* Clears Array from memory */
    virtual ~Array() {}

    /** INHERITED METHODS **/

    /* Inherited from Object, checks equality between an Array and an Object */
    virtual bool equals(Object *const obj) { return false; }

    /* Inherited from Object, generates a hash for an Array */
    virtual size_t hash() { return 0; }

    /** ARRAY METHODS **/

    /* Removes all elements from the Array */
    virtual void clear() {}

    /* Returns the current size of the contents in an Array */
    size_t size() { return this->len_; }

    /* Returns if the current array is full */
    bool is_full() { return this->cap_ == this->len_; }
};

/**
 * An Array class to which bools can be added to and removed from.
 */
class BoolArray : public Array
{
public:
    bool* arr_; // Represents the list of booleans in the array
    /** CONSTRUCTORS & DESTRUCTORS **/

    /** Default constructor */
    BoolArray() : BoolArray(cap_size) {}


    /* Creates an Array of desired size */
    BoolArray(const size_t size) {
        this->arr_ = new bool[size];
        this->cap_ = size;
        this->len_ = 0;
    }

    /* Copy Constructor */
    BoolArray(BoolArray& a): BoolArray (cap_size) {
        for (size_t i = 0; i < a.size(); i++) {
            this->add(a.get(i));
        }
    }

    /* Clears Array from memory */
    ~BoolArray() {
        delete[] this->arr_;
    }

    /**
     * Gets an bool at the given index
     * Throws an error if not found or out of range or no elements in array 
     * 
     * @arg index Index to get value at
     */
    bool get(size_t index) {
        assert(index < this->size());
        return this->arr_[index];
    }

    /**
     * Appends bool e to the end of this Array
     * 
     * @arg e boolean to add to the array
     */
    void add(bool e) {
        assert(!this->is_full());

        this->arr_[this->len_] = e;
        this->len_++;
    }
};

/**
 * An Array class where doubles can be added to and removed from.
 */
class DoubleArray : public Array
{
public:
    double* arr_; // Represents the list of doubles in the array

    /** CONSTRUCTORS & DESTRUCTORS **/

    /** Default constructor */
    DoubleArray(): DoubleArray (cap_size) {}

    /* Creates an Array of desired size */
    DoubleArray(const size_t size) {
        this->arr_ = new double[size];
        this->cap_ = size;
        this->len_ = 0;
    }

    /* Copy Constructor */
    DoubleArray(DoubleArray& a): DoubleArray (cap_size) {
        for (size_t i = 0; i < a.size(); i++) {
            this->add(a.get(i));
        }
    }

    /* Creates an Array from a serialized char array that has been converted to a String */
    DoubleArray(String* elements) {
        char *size = std::strtok(elements->c_str(), ";"); // get size from data
        int size_int = std::atoi(size); // convert char size to int
        this->arr_ = new double[cap_size];
        this->cap_ = cap_size;
        this->len_ = 0;

        if (strncmp(size, "0", 1) == 0) { // nothing in array, return empty array
           // do nothing
        } else {
            char *token = std::strtok(NULL, ","); // split on commas
            int count = 0;
            while (count < size_int && token != NULL) {
                add(std::stod(token));
                token = std::strtok(NULL, ",");
                count++;
            }
        }
    }

    /* Clears Array from memory */
    ~DoubleArray() {
        delete[] this->arr_;
    }

    /** ARRAY METHODS **/

    /**
     * Gets an double at the given index
     * Throws an error if not found or out of range or no elements in array 
     * Returns the index of the first occurence of the double.
     * 
     * @arg index to get element at
     */
    double get(size_t index) {
        assert(index < this->size());

        return this->arr_[index];
    }

    /** Returns the index of the given double, -1 if double is not found 
     * 
     * @arg o double to find in this array
    */
    size_t index_of(double o) {
        size_t idx = -1;
        for (size_t i = 0; i < this->size(); i++) {
            if (fabs(this->get(i) - o) < EPSILON) {
                idx = i;
                break;
            }
        }
        return idx;
    }

    /**
     * Appends double e to the end of this Array
     * 
     * @arg e double to add to the array
     */
    void add(double e) {
        assert(!this->is_full());

        this->arr_[this->len_] = e;
        this->len_++;
    }


    /** Replaces a double at the given index with the given double
     * Throws an error if not found or out of range or no elements in array
     * 
     * @arg index Index to replace new value with
     * @arg to_add New value to replace old value with
     */
    void replace(size_t index, double const to_add) {
        assert(index < this->size());
        this->arr_[index] = to_add;
    }

    // Method to serialize a double array
    char* serialize() {
        string all_doubles("1;"); // indicates that it is serializing a double array
        all_doubles += to_string(len_) + semicolon;

        if (size() != 0) {
            for (size_t i = 0; i < len_; i++) {
                all_doubles += to_string(get(i)) + comma;
            }
        }

        char* char_array = new char[all_doubles.length() + 1]; 
        strcpy(char_array, all_doubles.c_str());
        return char_array;
    }
};

/**
 * An Array class to which ints can be added to and removed from.
 */
class IntArray : public Array
{
public:
    int* arr_; // Represents the list of ints in the array

    /** CONSTRUCTORS & DESTRUCTORS **/

    /** Default constructor */
    IntArray(): IntArray(cap_size) { }

    /* Creates an Array of desired size */
    IntArray(const size_t size) {
        this->arr_ = new int[size];
        this->cap_ = size;
        this->len_ = 0;
    }

    /* Copy Constructor */
    IntArray(IntArray& a) : IntArray(cap_size) {
        for (size_t i = 0; i < a.size(); i++) {
            this->add(a.get(i));
        }
    }

    /* Clears Array from memory */
    ~IntArray() {
        delete[] this->arr_;
    }

    /** ARRAY METHODS **/

    /**
     * Gets an int at the given index
     * Throws an error if not found or out of range or no elements in array 
     * Returns the index of the first occurence of the integer
     * 
     * @arg index to get element at
     */
    int get(size_t index) {
        assert(index < this->size());
        return this->arr_[index];
    }

    /** Returns the index of the given int, -1 if int is not found 
     * 
     * @arg o int to find in array
    */
    size_t index_of(int o) {
        size_t idx = -1;
        for (size_t i = 0; i < this->size(); i++) {
            if (this->get(i) == o) {
                idx = i;
                break;
            }
        }
        return idx;
    }

    /**
     * Appends int e to the end of this Array
     * 
     * @arg e int to add to the array
     */
    void add(int e) {
        assert(!this->is_full());
        this->arr_[this->len_] = e;
        this->len_++;
    }

    /** Replaces a int at the given index with the given int
     * Throws an error if not found or out of range or no elements in array
     */
    void replace(size_t index, int const to_add) {
        assert(index < this->size());
        this->arr_[index] = to_add;
    }
};

/**
 * An Array class to which Strings can be added to and removed from.
 * author: chasebish */
class StringArray : public Array
{
public:
    String** arr_; // Represents the list of Strings in the array

    /** CONSTRUCTORS & DESTRUCTORS **/

    /** Default constructor */
    StringArray(): StringArray(cap_size) { }

    /* Creates an Array of desired size */
    StringArray(const size_t size) {
        this->arr_ = new String*[size];
        this->cap_ = size;
        this->len_ = 0;
    }

    /* Copy Constructor */
    StringArray(StringArray& a) : StringArray(cap_size) {
        for (size_t i = 0; i < a.size(); i++) {
            String* current = a.get(i);
            if (current == nullptr) {
                this->add(nullptr);
            } else {
                this->add(new String(a.get(i)->c_str()));
            } 
        }
    }

    /* Creates an Array from a serialized char array that has been converted to a String */
    // elements only includes size of array and actual elements
    StringArray(String* elements) {
        char *size = std::strtok(elements->c_str(), ";"); // get size from data
        int size_int = atoi(size); // convert char size to int
        this->arr_ = new String*[cap_size];
        this->cap_ = cap_size;
        this->len_ = 0;

        if (strncmp(size, "0", 1) == 0) { // nothing in array, return empty array
           // do nothing
        } else {
            char *token = std::strtok(NULL, ","); // split on commas
            int count = 0;
            while (count < size_int && token != NULL) {
                add(new String(token));
                token = std::strtok(NULL, ",");
                count++;
            }
        }
    }

    /* Clears Array from memory */
    ~StringArray() {
        for (size_t i = 0; i < size(); i++) {
            delete this->arr_[i];
        }
        delete[] this->arr_;
    }

    /** ARRAY METHODS **/

    /**
     * Gets an String* at the given index
     * Throws an error if not found or out of range or no elements in array 
     */
    String* get(size_t index) {
        assert(index < this->size());
        return this->arr_[index];
    }

    /** Returns the index of the given String, -1 if String is not found 
     * 
     * @arg o String to find the index of in the array
    */
    size_t index_of(String* o) {
        size_t idx = -1;
        for (size_t i = 0; i < this->size(); i++) {
            if (this->get(i)->equals(o)) {
                idx = i;
                break;
            }
        }
        return idx;
    }

    /**
     * Appends String e to the end of this Array
     * 
     * @arg e String to add to the array
     */
    void add(String* e) {
        if (this->is_full()) {
            this->allocate_();
        }
        this->arr_[this->len_] = e;
        
        this->len_++;
    }

    /** Replaces a String at the given index with the given String
     * Throws an error if not found or out of range or no elements in array
     */
    void replace(size_t index, String* const to_add) {
        assert(index < this->size());
        this->arr_[index] = to_add;
    }

    /** Inherited from Object, checks equality between an Array and an Object 
     * @arg obj Other object to compare this array to
     */
    bool equals(Object *const obj) {
        if (obj == nullptr) return false;
		StringArray* s = dynamic_cast<StringArray*>(obj);
		if (s == nullptr) return false;

		if (this->size() != s->size()) {
			return false;
		} else {
			for (size_t i = 0; i < this->size(); i++) {
				String* other_first_value = s->get(i);
				if (!this->arr_[i]->equals(other_first_value)) {
					return false;
				}
			}
			return true;
		}
    }

    /* Inherited from Object, generates a hash for an Array */
    virtual size_t hash() {
        size_t res = 0;
		for (size_t i = 0; i < this->len_; i++) {
			res += this->arr_[i]->hash();
		}
		return res;
    }

    // Allocates more space for the array by doubling when length is equal to capacity
    void allocate_() {
        if (this->len_ == this->cap_) {
            String** temp = new String*[2 * this->cap_];

            for (size_t i = 0; i < this->cap_; i++) {
                temp[i] = this->arr_[i];
            }
            delete[] this->arr_;
            this->arr_ = temp;
            this->cap_ *= 2;
        }
    }

    // Method to serialize a string array
    char* serialize() {
        string all_strings("0;"); // indicates that it is serializing a string array
        all_strings += to_string(len_) + semicolon;

        if (size() != 0) {
            for (size_t i = 0; i < len_; i++) {
                all_strings += string(get(i)->c_str()) + comma;
            }
        }

        char* char_array = new char[all_strings.length() + 1]; 
        strcpy(char_array, all_strings.c_str());
        return char_array;
    }
};

/**
 * An Array class to which Objects can be added to and removed from.
 */
class ObjectArray : public Array {
public:
    Object** arr_; // Represents the list of Objects

    /** Default constructor */
    ObjectArray() {
        this->arr_ = new Object*[cap_size];
        this->cap_ = cap_size;
        this->len_ = 0;
    }

    /* Creates an Array of desired size */
    ObjectArray(size_t size) {
        this->arr_ = new Object*[size];
        this->cap_ = size;
        this->len_ = 0;
    }

    /* Clears Array from memory */
    ~ObjectArray() {
        for (size_t i = 0; i < size(); i++) {
            delete this->arr_[i];
        }
        delete[] this->arr_;
    }

    /** ARRAY METHODS **/

    /**
     * Gets an Object* at the given index
     * Throws an error if not found or out of range or no elements in array 
     * Returns the index of the first occurence of the object.
     * 
     * @arg index to get element at
     */
    Object* get(size_t index) {
        assert(index < this->size());
        return this->arr_[index];
    }

    /** Returns the index of the given Object, -1 if Object is not found 
     * 
     * @arg o Object to find the index of in the array
    */
    size_t index_of(Object* o) {
        size_t idx = -1;
        for (size_t i = 0; i < this->size(); i++) {
            if (this->get(i)->equals(o)) {
                idx = i;
                break;
            }
        }
        return idx;
    }
    /**
     * Appends Object e to the end of this Array
     * 
     * @arg e Object to add to the array
     */
    void add(Object* e) {
        if (this->is_full()) {
            allocate_();
        }
        this->arr_[this->len_] = e;
        this->len_++;
    }

    /** Replaces a Object at the given index with the given Object
     * Throws an error if not found or out of range or no elements in array
     * 
     * @arg index Index to replace element at
     * @arg to_add New value to replace old value with
     */
    void replace(size_t index, Object* const to_add) {
        assert(index < this->cap_);
        if (this->arr_[index] != nullptr) {
            delete this->arr_[index];
        }
        
        this->arr_[index] = to_add;
    }

    // Allocates more space for the array by doubling when length is equal to capacity
    void allocate_() {
        if (this->len_ == this->cap_) {
            Object** temp = new Object*[2 * this->cap_];

            for (size_t i = 0; i < this->cap_; i++) {
                temp[i] = this->arr_[i];
            }
            delete[] this->arr_;
            this->arr_ = temp;
            this->cap_ *= 2;
        }
    }
};