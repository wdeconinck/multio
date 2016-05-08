/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Tiago Quintino
/// @date Dec 2015


#ifndef multio_MultIO_H
#define multio_MultIO_H

#include <iosfwd>
#include <string>
#include <vector>

#include "eckit/memory/NonCopyable.h"
#include "eckit/io/Length.h"

#include "multio/DataSink.h"
#include "multio/JournalRecord.h"

namespace multio {

//----------------------------------------------------------------------------------------------------------------------

class MultIO : public DataSink {

public:

    MultIO(const eckit::Configuration& config);

    virtual ~MultIO();

    virtual bool ready() const;

    /// Return the value that is serialised to produce the json() in the journal.
    /// For a MultIO, this ensures that the sub-sink configs are generated by the sub sinks
    /// rather than just using the passed configuration directly
    virtual eckit::Value configValue() const;

    virtual void write(eckit::DataBlobPtr blob);
    virtual void write(eckit::DataBlobPtr blob, JournalRecordPtr record);

    virtual void flush();

    virtual void replayRecord(const JournalRecord& record);

    void commitJournal();

    ///
    /// LEGACY INTERFACE TO REMOVE AFTER IFS CHANGED TO SIMPLE WRITE() INTERFACE
    ///

    virtual void iopenfdb(const std::string& name, const std::string& mode);
    virtual void iinitfdb();
    virtual void iclosefdb();

    virtual void isetcommfdb(int rank);
    virtual void isetrankfdb(int rank);
    virtual void iset_fdb_root(const std::string& name);

    virtual void iflushfdb();

    virtual void isetfieldcountfdb(int all_ranks, int this_rank);
    virtual void isetvalfdb(const std::string& name, const std::string& value);

protected: // types

    typedef std::vector<DataSink*> sink_store_t;

protected:

    virtual void print(std::ostream&) const;

protected: // members

    Journal journal_;

    sink_store_t sinks_;

private: // methods

    friend std::ostream &operator<<(std::ostream &s, const MultIO &p) {
        p.print(s);
        return s;
    }

};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace multio

#endif

