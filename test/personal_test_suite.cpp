#include "../src/util/array.h"
#include "../src/client/column.h"
#include "../src/client/schema.h"
#include "../src/client/row.h"
#include "../src/client/dataframe.h"
#include "../src/util/helper.h"
// authors: wang.ang@husky.neu.edu, mattie.r@husky.neu.edu
void FAIL() { exit(1); }
void OK(const char *m)
{
    Sys *c = new Sys();
	c->p("OK: ");
	c->pln(m);
    delete c;
}
void t_true(bool p)
{
    if (!p)
        FAIL();
}
void t_false(bool p)
{
    if (p)
        FAIL();
}

// ARRAY TESTS
void testBoolArray()
{
    // test add
    bool s = true;
    bool t = false;
    bool u = true;
    BoolArray *arr1 = new BoolArray();
    t_true(arr1->size() == 0);
    arr1->add(t);
    arr1->add(s);
    arr1->add(u);
    t_true(arr1->size() == 3);
    // OK("Boolean Array tests - add");

    // test get
    t_true(arr1->get(0) == t);
    t_true(arr1->get(1) == s);
    t_true(arr1->get(2) == u);
    // OK("Boolean Array tests - get");

    delete arr1;
    // OK("Boolean Array tests");
    OK(".");

}


void testDoubleArray()
{
    // test add
    double s = 1.1;
    double t = 2.10;
    double u = 3.3;
    double v = 4.4;
    DoubleArray *arr = new DoubleArray();
    t_true(arr->size() == 0);
    arr->add(t);
    arr->add(s);
    arr->add(u);
    t_true(arr->size() == 3);
    // OK("double Array tests - add");

    // test get
    t_true(arr->get(0) == t);
    t_true(arr->get(1) == s);
    t_true(arr->get(2) == u);
    // OK("double Array tests - get");

    // test index of
    t_true(arr->index_of(t) == 0);
    t_true(arr->index_of(s) == 1);
    t_true(arr->index_of(v) == (size_t)-1);
    // OK("double Array tests - index of");

    // test replace
    arr->replace(0, t);
    t_true(arr->get(0) == t);
    arr->replace(0, s);
    // OK("double Array tests - replace");

    delete arr;
    // OK("double Array tests");
    OK(".");

}

void testIntArray()
{
    // test add
    int s = 1;
    int t = 2;
    int u = 3;
    int v = 4;
    IntArray *arr = new IntArray();
    t_true(arr->size() == 0);
    arr->add(t);
    arr->add(s);
    arr->add(u);
    t_true(arr->size() == 3);
    // OK("Integer Array tests - add");

    // test get
    t_true(arr->get(0) == t);
    t_true(arr->get(1) == s);
    t_true(arr->get(2) == u);
    // OK("Integer Array tests - get");

    // test index of
    t_true(arr->index_of(t) == 0);
    t_true(arr->index_of(s) == 1);
    t_true(arr->index_of(v) == (size_t)-1);
    // OK("Integer Array tests - index of");

    // test replace
    arr->replace(0, t);
    t_true(arr->get(0) == t);
    arr->replace(0, s);
    // OK("Integer Array tests - replace");

    delete arr;
    // OK("Integer Array tests");
    OK(".");

}

void testStringArray()
{
    // test add
    String *s = new String("Hello");
    String *t = new String("World");
    String *u = new String("!");
    String *v = new String("Bye");
    StringArray *arr = new StringArray();
    t_true(arr->size() == 0);
    arr->add(t);
    arr->add(s);
    arr->add(u);
    t_true(arr->size() == 3);
    // OK("String Array tests - add");

    // test get
    t_true(arr->get(0)->equals(t));
    t_true(arr->get(1)->equals(s));
    t_true(arr->get(2)->equals(u));
    // OK("String Array tests - get");

    // test index of
    t_true(arr->index_of(t) == 0);
    t_true(arr->index_of(s) == 1);
    t_true(arr->index_of(v) == (size_t)-1);
    // OK("String Array tests - index of");

    StringArray* big = new StringArray();
    for (size_t i = 0; i < 10; i++) {
        String* sbig = new String("hey");
        big->add(sbig);
    }
    t_true(big->size() == 10);
    // OK("String Array tests - large array");

    delete big;
    delete v;
    delete arr;
    // OK("String Array tests");
    OK(".");

}


void testIntColumn()
{
    // simple column test
    IntColumn *col = new IntColumn();

    col->push_back(1);
    col->push_back(2);
    col->push_back(3);
    col->push_back(4);
    t_true(col->get(0) == 1);
    t_true(col->get(1) == 2);
    t_true(col->get(2) == 3);
    t_true(col->get(3) == 4);
    t_true(col->size() == 4);

    // test get type
    t_true(col->get_type() == 'I');

    Column* general_col = dynamic_cast<Column*>(col);
    t_true(general_col->as_int()->size() != 0);
    t_true(general_col->as_double() == nullptr);
    t_true(general_col->as_bool() == nullptr);
    t_true(general_col->as_string() == nullptr);
    // OK("Int Column tests - as type");
    
    IntColumn *col_copy = new IntColumn(*col);
    t_true(col->get(0) == col_copy->get(0));
    t_true(col->get(1) == col_copy->get(1));
    t_true(col->get(2) == col_copy->get(2));
    // OK("Int Column tests - copy constructor");

    delete col;
    delete col_copy;

    // OK("Int Column tests");
    OK(".");

}

void testBoolColumn()
{
    // simple column test
    BoolColumn *col = new BoolColumn();

    col->push_back(true);
    col->push_back(false);
    col->push_back(true);
    col->push_back(true);
    t_true(col->get(0) == true);
    t_true(col->get(1) == false);
    t_true(col->get(2) == true);
    t_true(col->get(3) == true);
    t_true(col->size() == 4);

    // test get type
    t_true(col->get_type() == 'B');

    Column* general_col = dynamic_cast<Column*>(col);
    t_true(general_col->as_int() == nullptr);
    t_true(general_col->as_double() == nullptr);
    t_true(general_col->as_string() == nullptr);
    // OK("Bool Column tests - as type");

    BoolColumn *col_copy = new BoolColumn(*col);
    t_true(col->get(0) == col_copy->get(0));
    t_true(col->get(1) == col_copy->get(1));
    t_true(col->get(2) == col_copy->get(2));
    // OK("Bool Column tests - copy constructor");

    delete col;
    delete col_copy;

    // OK("Bool Column tests");
    OK(".");

}

void testDoubleColumn()
{
    // simple column test
    DoubleColumn *col = new DoubleColumn();

    col->push_back(1.1);
    col->push_back(2.2);
    col->push_back(3.3);
    col->push_back(4.4);
    t_true(col->get(0) == (double)1.1);
    t_true(col->get(1) == (double)2.2);
    t_true(col->get(2) == (double)3.3);
    t_true(col->get(3) == (double)4.4);
    t_true(col->size() == 4);

    // test get type
    t_true(col->get_type() == 'F');
    
    Column* general_col = dynamic_cast<Column*>(col);
    t_true(general_col->as_int() == nullptr);
    t_true(general_col->as_bool() == nullptr);
    t_true(general_col->as_string() == nullptr);
    // OK("Double Column tests - as type");

    DoubleColumn *col_copy = new DoubleColumn(*col);
    t_true(col->get(0) == col_copy->get(0));
    t_true(col->get(1) == col_copy->get(1));
    t_true(col->get(2) == col_copy->get(2));
    // OK("Double Column tests - copy constructor");

    delete col;
    delete col_copy;

    // OK("Double Column tests");
    OK(".");

}

void testStringColumn()
{
    // simple column test
    StringColumn *col = new StringColumn();
    String *s = new String("Hello");
    String *t = new String("World");
    String *u = new String("!");
    String *v = new String("Bye");

    col->push_back(s);
    col->push_back(t);
    col->push_back(u);
    col->push_back(v);
    t_true(col->get(0)->equals(s));
    t_true(col->get(1)->equals(t));
    t_true(col->get(2)->equals(u));
    t_true(col->get(3)->equals(v));
    t_true(col->size() == 4);
    // OK("String Column tests - pushback");

    // test get type
    t_true(col->get_type() == 'S');
    

    Column* general_col = dynamic_cast<Column*>(col);
    t_true(general_col->as_int() == nullptr);
    t_true(general_col->as_double() == nullptr);
    t_true(general_col->as_bool() == nullptr);
    // OK("Double Column tests - as type");

    StringColumn *col_copy = new StringColumn(*col);
    t_true(col->get(0)->equals(col_copy->get(0)));
    t_true(col->get(1)->equals(col_copy->get(1)));
    t_true(col->get(2)->equals(col_copy->get(2)));
    // OK("String Column tests - copy constructor");
    delete col;
    delete col_copy;
    // OK("String Column tests");
    OK(".");

}

void testSchema() {
    String* si = new String("int column");

    Schema* s1 = new Schema();

    s1->add_column('I', si);
    s1->add_column('S', si);
    s1->add_column('F', si);
    s1->add_column('B', si);
    t_true(s1->width() == 4);
    // OK("Schema - add_column");

    t_true(s1->col_type(0) == 'I');
    t_true(s1->col_type(1) == 'S');
    t_true(s1->col_type(2) == 'F');
    t_true(s1->col_type(3) == 'B');
    // OK("Schema - col_type");

    s1->add_row(si);
    s1->add_row(si);
    s1->add_row(si);
    t_true(s1->length() == 3);
    // OK("Schema - add_row");

    Schema* s1_copy = new Schema(*s1);
    t_true(s1->equals(s1_copy));
    t_true(s1_copy->equals(s1));
    // OK("Schema - copy constructor");

    s1->add_row(si);
    t_false(s1->equals(s1_copy));
    // OK("Schema - changing variables do not change new schema");

    Schema* s2 = new Schema("ISBFI");
    t_true(s2->width() == 5);
    t_true(s2->col_type(0) == 'I');
    t_true(s2->col_type(1) == 'S');
    t_true(s2->col_type(2) == 'B');
    t_true(s2->col_type(4) == 'I');
    // OK("Schema - constructor with types");

    delete si;
    delete s1;
    delete s1_copy;
    delete s2;
    OK(".");

}

void testRow() {
    char* types = const_cast<char*>("IBFSI");
    Schema* s = new Schema(types); // TODO: Add this

    Row* r = new Row(*s);
    r->set_idx(0);
    t_true(r->get_idx() == 0);
    t_true(r->width() == 5);
    t_true(r->col_type(0) == 'I');
    t_true(r->col_type(1) == 'B');
    t_true(r->col_type(2) == 'F');
    t_true(r->col_type(3) == 'S');
    t_true(r->col_type(4) == 'I');
    // OK("Row tests - column types");


    r->set(0, 1);
    r->set(1, true);
    r->set(2, (double)2.2);
    String expected("3sss");
    r->set(3, &expected);
    r->set(4, 4);
    t_true(r->width() == 5);

    t_true(r->get_int(0) == 1);
    t_true(r->get_bool(1) == true);
    t_true(r->get_double(2) == (double)2.2);
    t_true(r->get_string(3)->equals(&expected));
    t_true(r->get_int(4) == 4);
    // OK("Row tests - set and get");

    r->set(0, 10);
    t_true(r->get_int(0) == 10);

    delete s;
    delete r;
    // OK("Row tests");
    OK(".");

}

void testDataFrame() {
    String* si = new String("int column");
    
    Schema* s1 = new Schema();
    s1->add_column('I', si);
    s1->add_column('S', si);
    s1->add_column('F', si);
    s1->add_column('B', si);
    s1->add_row(si);
    s1->add_row(si);
    s1->add_row(si);

    DataFrame* d1 = new DataFrame(*s1);
    // OK("DataFrame - constructor with schema");

    IntColumn* c1 = new IntColumn();
    c1->push_back(1);
    c1->push_back(2);
    c1->push_back(3);

    StringColumn *c2 = new StringColumn();
    String *s = new String("Hello");
    String *t = new String("World");
    String *u = new String("!");
    c2->push_back(s);
    c2->push_back(t);
    c2->push_back(u);
    DoubleColumn *c3 = new DoubleColumn();
    c3->push_back(1.1);
    c3->push_back(2.2);
    c3->push_back(3.3);

    BoolColumn *c4 = new BoolColumn();
    c4->push_back(true);
    c4->push_back(true);
    c4->push_back(false);

    d1->add_column(c1, si);
    d1->add_column(c2, si);
    d1->add_column(c3, si);
    d1->add_column(c4, si);
    t_true(d1->ncols() == 4);
    t_true(d1->nrows() == 3);
    // OK("DataFrame - added columns");

    t_true(d1->get_int(0, 1) == 2);
    t_true(d1->get_string(1, 0)->equals(s));
    t_true(d1->get_double(2,2) == (double)3.3);
    t_true(d1->get_bool(3, 2) == false);
    // OK("DataFrame - get elements");

    Row* row1 = new Row(*s1);
    Row* row2 = new Row(*s1);

    d1->fill_row(0, *row1);
    d1->fill_row(1, *row2);

    t_true(row1->get_int(0) == 1);
    String hello("Hello");
    t_true(row1->get_string(1)->equals(&hello));
    t_true(row1->get_double(2) == (double)1.1);
    t_true(row1->get_bool(3) == true);

    t_true(row2->get_int(0) == 2);
    String world("World");
    t_true(row2->get_string(1)->equals(&world));
    t_true(row2->get_double(2) == (double)2.2);
    t_true(row2->get_bool(3) == true);

    // OK("DataFrame - fill_row");

    d1->add_row(*row1);
    t_true(d1->nrows() == 4);
    d1->add_row(*row2);
    t_true(d1->nrows() == 5);

    t_true(d1->get_int(0,4) == 2);
    t_true(d1->get_string(1,3)->equals(&hello));
    t_true(d1->get_double(2,4) == (double)2.2);
    t_true(d1->get_bool(3,3) == true);
    // OK("DataFrame - add_row");

    d1->add_row(*row2);
    d1->add_row(*row2);
    d1->add_row(*row2);
    d1->add_row(*row2);
    d1->add_row(*row2);
    t_true(d1->nrows() == 10);
    
    delete row1;
    delete row2;
    delete d1;
    delete s1;
    delete c1;
    delete c2;
    delete c3;
    delete c4;
    delete si;

    // OK("DataFrame - tests");
    OK(".");

}

void testGetLarge() {
    Schema s("II");

    DataFrame df(s);
    Row  r(df.get_schema());
    for(size_t i = 0; i <  1000 * 1000; i++) {
        r.set(0,(int)i);
        r.set(1,(int)i+1);
        df.add_row(r);
    }
    t_true(df.get_int((size_t)0,1023) == 1023);
    t_true(df.get_int((size_t)0,1024) == 1024);
    // OK("Test getting large index");
    OK(".");
}


int main()
{
    testBoolArray();
    testDoubleArray();
    testIntArray();
    testStringArray();
    testIntColumn();
    testBoolColumn();
    testDoubleColumn();
    testStringColumn();
    testSchema();
    testRow();
    testDataFrame();
    testGetLarge();
    OK("all tests passed");

    return 0;
}