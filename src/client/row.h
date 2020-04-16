// Lang::CwC
#pragma once
#include "../util/wrapper.h"

/*************************************************************************
 * Row::
 *
 * This class represents a single row of data constructed according to a
 * dataframe's schema. The purpose of this class is to make it easier to add
 * read/write complete rows. Internally a dataframe hold data in columns.
 * Rows have pointer equality.
 * 
 * authors: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
 */
class Row : public Object {
public:
    Schema* schema_;
    size_t index_;
    ObjectArray* elements_;

    /** Build a row following a schema. */
    Row(Schema& scm) {
        this->schema_ = new Schema(scm);
        this->elements_ = new ObjectArray(scm.width());
        for (size_t i = 0; i < scm.width(); i++) {
            this->elements_->add(new Object());
        }
        this->index_ = -1;
    }

    /** Destructor */
    ~Row() {
        delete this->schema_;
        delete this->elements_;
    }

    /** Setters: set the given column with the given value. Setting a column with
     * a value of the wrong type is undefined. 
     * Setting when a value already exists should overwrite value 
     */
    void set(size_t col, int val) {
        char colType = this->schema_->col_type(col); // if out of range, will be caught by col_type and error
        assert(colType == 'I');

        this->elements_->replace(col, new Integer(val));
    }
    void set(size_t col, double val) {
        char colType = this->schema_->col_type(col); // if out of range, will be caught by col_type and error
        assert(colType == 'F');

        this->elements_->replace(col, new Double(val));
    }
    void set(size_t col, bool val) {
        char colType = this->schema_->col_type(col); // if out of range, will be caught by col_type and error
        assert(colType == 'B');

        this->elements_->replace(col, new Boolean(val));
    }
    /** Acquire ownership of the string. */
    void set(size_t col, String* val) {
        char colType = this->schema_->col_type(col); // if out of range, will be caught by col_type and error
        assert(colType == 'S');

        this->elements_->replace(col, new String(*val));
    }

    /** Set/get the _ of this row (ie. its position in the dataframe. This is
     *  only used for informational purposes, unused otherwise */
    void set_idx(size_t idx) {
        this->index_ = idx;
    }
    size_t get_idx() { return this->index_; }

    /** Getters: get the value at the given column. If the column is not
    * of the requested type, the result is undefined. */
    int get_int(size_t col) {
        assert(this->schema_->col_type(col) == 'I');

        Object* o = this->elements_->get(col);
        assert(o != nullptr);

        Integer* s = dynamic_cast<Integer*>(o);
        assert(s != nullptr);
        return (int)s->get();
    }
    bool get_bool(size_t col) {
        assert(this->schema_->col_type(col) == 'B');

        Object* o = this->elements_->get(col);
        assert(o != nullptr);

        Boolean* s = dynamic_cast<Boolean*>(o);
        assert(s != nullptr);
        return (bool)s->get();
    }
    double get_double(size_t col) {
        assert(this->schema_->col_type(col) == 'F');

        Object* o = this->elements_->get(col);
        assert(o != nullptr);

        Double* s = dynamic_cast<Double*>(o);
        assert(s != nullptr);
        return (double)s->get();
    }
    String* get_string(size_t col) {
        assert(this->schema_->col_type(col) == 'S');

        Object* o = this->elements_->get(col);
        assert(o != nullptr);

        String* s = dynamic_cast<String*>(o);
        assert(s != nullptr);
        return s;
    }

    /** Number of fields in the row. */
    size_t width() { return this->schema_->width(); }

    /** Type of the field at the given position. An idx >= width is  undefined. */
    char col_type(size_t idx) {
        assert(idx < this->schema_->width());
        return this->schema_->col_type(idx);
    }

    /** Returns if all fields have values in it */
    bool all_fields_set_() {
        bool all_set = true;

        for (size_t i = 0; i < this->schema_->width(); i++) {
            if (this->elements_->get(i) == nullptr) {
                all_set = false;
                break;
            }
        }
        return all_set;
    }
};