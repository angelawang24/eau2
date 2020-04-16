//lang::CwC
#pragma once
#include "column.h"
#include "schema.h"
#include "../store/keyvaluestore.h"
#include "../store/writer.h"
#include "../store/reader.h"
#include <mutex>
using namespace std;
/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A DataFrame has a schema that
 * describes it.
 * 
 * author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
 */
mutex waitlock;
class DataFrame : public Object {
public:
    ObjectArray* columns_; // represents the columns
    Schema* schema_; // represents the schema for the DataFrame
    size_t num_rows_; // number of rows
    size_t num_cols_; // number of columns

 
    /** Create a data frame with the same columns as the give df but no rows or rownames */
    DataFrame(DataFrame& df) {
        this->schema_ = new Schema(df.get_schema());
        this->columns_ = new ObjectArray(this->schema_->width());
        this->num_rows_ = 0;
        this->num_cols_ = 0;

        for (size_t i = 0; i < schema_->width(); i++) {
            char type = schema_->col_type(i);
            if (type == 'I') { this->columns_->add(new IntColumn()); } 
            else if (type == 'F') { this->columns_->add(new DoubleColumn()); } 
            else if (type == 'B') { this->columns_->add(new BoolColumn()); } 
            else if (type == 'S') { this->columns_->add(new StringColumn()); }
        }
    }

    /** Create a data frame from a schema and columns. All columns are created empty */
    DataFrame(Schema& schema) {
        this->schema_ = new Schema(schema);
        this->columns_ = new ObjectArray(schema.width());
        this->num_rows_ = 0;
        this->num_cols_ = 0;
        for (size_t i = 0; i < schema.width(); i++) {
            char type = schema.col_type(i);
            if (type == 'I') { this->columns_->add(new IntColumn()); } 
            else if (type == 'F') { this->columns_->add(new DoubleColumn()); } 
            else if (type == 'B') { this->columns_->add(new BoolColumn()); } 
            else if (type == 'S') { this->columns_->add(new StringColumn()); }
        }
    }

    // Constructor used for deserializing, receives a String representing the serialized data
    // fields does not include type (3) and schema
    DataFrame(Schema& schema, String* fields) : DataFrame(schema) {
        // 2;0,1,;hey,hey,;1,1,;1.20000,1.20000,;
        char* rest = fields->c_str();
        char *token = strtok_r(rest, ";", &rest);
        int numrows = atoi(token);
        // adds empty rows to schema so that dataframe can add rows (has to be consistent)
        String mt("");
        for (int x = 0; x < numrows; x++) {
            schema_->add_row(&mt);
        }

        //gets the first column from the serialized data
        token = strtok_r(rest, ";", &rest);
        // for each column
        for (size_t i = 0; i < schema.width() - 1; i++) {
            char type = schema.col_type(i);
            // add column
            Column* newcol = create_column_(new String(token), type);
            add_column(newcol, &mt);
            delete newcol;
            token = strtok_r(rest, ";", &rest);
        }

        // add the last column --> outside of loop because of the way strtok_r is
        char type = schema.col_type(schema.width() - 1);
        Column* newcol = create_column_(new String(token), type);
        add_column(newcol, &mt);
        delete newcol;
    }

    ~DataFrame() {
        delete this->schema_;
        delete this->columns_;
    }

    // Creates a column when deserializing based off of data from token and type passed in
    Column* create_column_(String* token, char type) {
        Column* c = nullptr;
        if (type == 'I') {
            c = new IntColumn();
            char *column_token = strtok(token->c_str(), ",");
            while (column_token != NULL) {
                c->push_back(atoi(column_token));
                column_token = strtok(NULL, ",");
            }
        } else if (type == 'F') {
            c = new DoubleColumn();
            char *column_token = strtok(token->c_str(), ",");
            while (column_token != NULL) {
                c->push_back(atof(column_token));
                column_token = strtok(NULL, ",");
            }
        } else if (type == 'B') {
            c = new BoolColumn();
            char *column_token = strtok(token->c_str(), ",");
            while (column_token != NULL) {
                c->push_back((bool)atoi(column_token));
                column_token = strtok(NULL, ",");
            }
        } else if (type == 'S') {
            c = new StringColumn();
            char *column_token = strtok(token->c_str(), ",");
            while (column_token != NULL) {
                c->push_back(new String(column_token));
                column_token = strtok(NULL, ",");
            }
        }
        delete token;
        return c;
    }

    /** Returns the dataframe's schema. Modifying the schema after a dataframe
    * has been created in undefined. */
    Schema& get_schema() { return *this->schema_; }

    /** Adds a column this dataframe, updates the schema, the new column
    * is external, and appears as the last column of the dataframe, the
    * name is optional and external. A nullptr colum is undefined. */
    void add_column(Column* col, String* name) {
        assert(col != nullptr);
        assert(this->schema_->length() == col->size());

        char type = col->get_type();
        // String* name_copy = new String(*name);
        if (this->schema_->width() == this->num_cols_) {
            this->schema_->add_column(type, name);
        }
        char schemaType = this->schema_->col_type(this->num_cols_);
        assert(type == 'I' || type == 'F' || type == 'S' || type == 'B');

        if (type == 'I') {
            IntColumn* cast_col = col->as_int();
            assert(cast_col != nullptr);

            IntColumn* copy_col = new IntColumn(*cast_col);
            assert(schemaType == type);

            this->columns_->replace(num_cols_, copy_col);
            this->num_cols_++;
        }
        else if (type == 'F') {
            DoubleColumn* cast_col = col->as_double();
            assert(cast_col != nullptr);

            DoubleColumn* copy_col = new DoubleColumn(*cast_col);
            assert(schemaType == type);

            this->columns_->replace(num_cols_, copy_col);
            this->num_cols_++;
        }
        else if (type == 'B') {
            BoolColumn* cast_col = col->as_bool();;
            assert(cast_col != nullptr);

            BoolColumn* copy_col = new BoolColumn(*cast_col);
            assert(schemaType == type);

            this->columns_->replace(num_cols_, copy_col);
            this->num_cols_++;
        }
        else if (type == 'S') {
            StringColumn* cast_col = col->as_string();
            assert(cast_col != nullptr);

            StringColumn* copy_col = new StringColumn(*cast_col);
            assert(schemaType == type);

            this->columns_->replace(num_cols_, copy_col);
            this->num_cols_++;
        } 
        if (col->size() > this->num_rows_ ) {
            this->num_rows_ = col->size();
        }
    }

    Column* get_column_(size_t col) {
        Object* obj_col = this->columns_->get(col);
        assert(obj_col != nullptr);

		Column* column = dynamic_cast<Column*>(obj_col);
        assert(column != nullptr);

        assert(column->cap_ == 1024);
        return column;
    }

    /** Return the value at the given column and row. Accessing rows or
     *  columns out of bounds, or request the wrong type is undefined.*/
    int get_int(size_t col, size_t row) {
        Column* column = this->get_column_(col);

        IntColumn* int_column = column->as_int();
        assert(int_column != nullptr);

        return int_column->get(row);
    }
    bool get_bool(size_t col, size_t row) {
        Column* column = this->get_column_(col);

        BoolColumn* bool_column = column->as_bool();
        assert(bool_column != nullptr);

        return bool_column->get(row);
    }
    double get_double(size_t col, size_t row) {
        Column* column = this->get_column_(col);

        DoubleColumn* double_column = column->as_double();
        assert(double_column != nullptr);

        return double_column->get(row);
    }
    String* get_string(size_t col, size_t row) {
        Column* column = this->get_column_(col);

        StringColumn* string_column = column->as_string();
        assert(string_column != nullptr);

        return string_column->get(row);
    }

    /** Set the fields of the given row object with values from the columns at
    * the given offset.  If the row is not form the same schema as the
    * dataframe, results are undefined.
    */
    void fill_row(size_t idx, Row& row) {
        assert(this->schema_->width() == row.width());

        for (size_t i = 0; i < this->schema_->width(); i++) { // iterate through column
            char type = this->schema_->col_type(i);
    
            if (type == 'I') row.set(i, this->get_int(i, idx));
            else if (type == 'F') row.set(i, this->get_double(i, idx));
            else if (type == 'B') row.set(i, this->get_bool(i, idx));
            else if (type == 'S') row.set(i, this->get_string(i, idx));          
        }
    }

    /** Add a row at the end of this dataframe. The row is expected to have
     *  the right schema and be filled with values, otherwise undedined.  */
    void add_row(Row& row) {
        assert(this->schema_->width() == row.width());

        if (this->num_cols_ < this->schema_->width()) {
            this->num_cols_ = this->schema_->width();
        }
        if (this->num_rows_ >= this->schema_->length()) {
            this->schema_->add_row(nullptr); 
        }
        for (size_t i = 0; i < row.width(); i++) {
            char row_element_type = row.col_type(i);
            assert(this->schema_->col_type(i) == row_element_type);

            Column* col = this->get_column_(i);
            
            if (row_element_type == 'I') col->push_back(row.get_int(i));
            else if (row_element_type == 'F') col->push_back(row.get_double(i));
            else if (row_element_type == 'B') col->push_back(row.get_bool(i));
            else if (row_element_type == 'S') {
                String* copy_string = new String(row.get_string(i)->c_str());
                col->push_back(copy_string);
            }
        }
        this->num_rows_++;
    }

    /** The number of rows in the dataframe. */
    size_t nrows() { return this->num_rows_; }

    /** The number of columns in the dataframe.*/
    size_t ncols() { return this->num_cols_; }

    /** Visit rows in order */
    void map(Reader& r) {
        Row new_row(this->get_schema());
        for (size_t i = 0; i < this->nrows(); i++) {
            this->fill_row(i, new_row);
            r.visit(new_row);
        }
    }

    /** Visit rows in specific range */
    void local_map(Reader& r, size_t i) {
        waitlock.lock();
        size_t start = i * this->nrows() / 3;
        size_t end = (i + 1) * this->nrows() / 3;
        Row new_row(this->get_schema());
        for (size_t i = start; i < end; i++) {
            this->fill_row(i, new_row);
            r.visit(new_row);
        }
        waitlock.unlock();
    }

    // DataFrame* df = DataFrame::fromArray(&key, &kv, SZ, vals);
    // Adds values to the kv store with the key provided, returns dataframe created from vals
    static DataFrame* fromArray(Key* key, KeyValueStore* kv, size_t SZ, double* vals) {
        // Create Dataframe from provided info
        Schema s("F");
        DataFrame* df = new DataFrame(s);
        Row r(df->get_schema());
        
        for (size_t i = 0; i < SZ; i++) {
            r.set(0, vals[i]);
            df->add_row(r);
        }
        //add this dataframe as a serialized blob to the kvstore provided
        kv->put(*key, df->serialize());
        return df;
    }

    // DataFrame* df = DataFrame::fromScalar(&key, &kv, scalar);
    // Adds scalar value to the kv store with the key provided, returns dataframe created from scalar
    static DataFrame* fromScalar(Key* key, KeyValueStore* kv, double scalar) {
        // Create Dataframe from provided info
        Schema s("F");
        DataFrame* df = new DataFrame(s);
        Row r(df->get_schema());
        r.set(0, scalar);
        df->add_row(r);

        kv->put(*key, df->serialize());

        return df;
    }

    // Calls visit on the Writer passed in, where Writer fills in the row.
    // The row gets added to a new DataFrame and gets saved in the kvstore.
    static DataFrame* fromVisitor(Key* key, KeyValueStore* kv, char* schema, Writer& fr) {
        Schema s(schema);
        DataFrame* df = new DataFrame(s);

        Row r(s);
        while (!fr.done()) {
            fr.visit(r);
            df->add_row(r);
        }
        kv->put(*key, df->serialize());
        return df;
    }

    // Method to serialize a dataframe, with this format:
    // 3;schema;num_rows;column1_element1,column1_element2...,;column2_element1,column2_element2...,;
    char* serialize() { 
        string msg_serialize("3;");
        string elements(";");
        string commastr(",");
        string semi(";");
        
        // Goes through each column
        for (size_t i = 0; i < schema_->width(); i++) {
            // Type of the column grabbed from schema
            char type = schema_->col_type(i);

            for (size_t j = 0; j < nrows(); j++) { // add all elements in column
                if (type == 'I') { elements += to_string(get_int(i, j)) + commastr; }
                else if (type == 'F') { elements += to_string(get_double(i, j)) + commastr; }
                else if (type == 'B') { elements += to_string(get_bool(i, j)) + commastr; }
                else if (type == 'S') { elements += string(get_string(i, j)->c_str()) + commastr; }
            }
            elements += semi;
            msg_serialize += string(1, type);
        }

        // add row number to serialized message
        msg_serialize += semi + to_string(nrows()) + elements;
        
        char* char_array = new char[msg_serialize.length() + 1]; 
        strcpy(char_array, msg_serialize.c_str());
        return char_array;
    }
};

