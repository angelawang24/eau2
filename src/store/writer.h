// Lang::CwC
#pragma once

#include "../client/row.h"
/*****************************************************************************
 * Writer::
 * A vistor invoked by Row.
 * author: instructors of CS4500
 */
class Writer : public Object
{
public:
    /** Reads next and stores it in the row. */
    virtual void visit(Row &r) {}

    /** Returns true when there are no more words to read.  There is nothing
       more to read if we are at the end of the buffer and the file has
       all been read. */
    virtual bool done() {
        return false;
    }
};