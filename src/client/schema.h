// Lang::CwC
#pragma once

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 *  author: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
 */

class Schema : public Object {
public:
    size_t numCol_; //number of columns in the data frame
    size_t numRow_; //number of rows in the data frame
    StringArray* cTypes_;  //a list of the types of each column in the data frame

    /** Copying constructor */
    Schema(Schema& from) {
        this->numCol_ = from.width();
        this->numRow_ = from.length();
        this->cTypes_ = new StringArray(*from.cTypes_);
    }

    /** Create an empty schema **/
    Schema() {
        this->numCol_ = 0;
        this->numRow_ = 0;
        this->cTypes_ = new StringArray();
    }

    /** Destructor */
    ~Schema() {
        delete this->cTypes_;
    }

    /** Create a schema from a string of types. A string that contains
      * characters other than those identifying the four type results in
      * undefined behavior. The argument is external, a nullptr argument is
      * undefined. **/
    Schema(const char* types) {
        this->cTypes_ = new StringArray();
        //Check that each element in types is a valid type before adding to cTypes_
        size_t index = 0;

        while(types[index] != '\0') {
            if(this->is_valid_type_(types[index])) {
                cTypes_->add(new String(&types[index]));
            }
            index++;
        }

        this->numCol_ = cTypes_->size();
        this->numRow_ = 0; 
    }

    /** Add a column of the given type and name (can be nullptr), name
      * is external. 
      * Can only have unique names, if name already exists, error will be thrown
      */
    void add_column(char typ, String* name) {
        assert(is_valid_type_(typ));

        this->numCol_++;
        const char *b = &typ;
        this->cTypes_->add(new String(b, 1));
    }

    /** Add a row with a name (possibly nullptr), name is external. 
      * Can only have unique names, if name already exists, error will be thrown
      */
    void add_row(String* name) { this->numRow_++; }

    /** Return type of column at idx. An idx >= width is undefined. */
    char col_type(size_t idx) {
        assert(idx < this->width());
        return this->cTypes_->get(idx)->at(0);
    }

    /** The number of columns */
    size_t width() { return this->numCol_; }

    /** The number of rows */
    size_t length() { return this->numRow_; }

    /** whether this char is a valid type **/
    bool is_valid_type_(char type) {
      return type == 'S' || type == 'B' || type == 'F' || type == 'I';
    }

    /** Inherited from Object, checks equality between a Schema and an Object 
     * @arg obj Other object to compare this array to
     */
    bool equals(Object *const obj) {
        if (obj == nullptr) return false;
		Schema* s = dynamic_cast<Schema*>(obj);
		if (s == nullptr) return false;

		if (this->width() != s->width() || this->length() != s->length()) {
			return false;
		} 
        else {
            // check for equality between column types
			for (size_t i = 0; i < this->width(); i++) {
				char other_first_value = s->col_type(i);
				if (this->col_type(i) != other_first_value) {
					return false;
				}
			}
			return true;
		}
    }
};