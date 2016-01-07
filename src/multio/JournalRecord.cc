/*
 * (C) Copyright 1996-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Simon Smart
/// @date Dec 2015

#include <sys/time.h>

#include "eckit/io/Buffer.h"
#include "eckit/io/DataHandle.h"

#include "multio/Journal.h"
#include "multio/JournalRecord.h"
#include "multio/SharableBuffer.h"

using namespace eckit;

namespace multio {

// -------------------------------------------------------------------------------------------------

const FixedString<4> JournalRecord::TerminationMarker("END!");
const unsigned char JournalRecord::CurrentTagVersion = 1;

const std::string JournalRecord::RecordTypeNames[] = {
    "Uninitialised",
    "End of Journal",
    "Journal entry",
    "Configuration"
};


// -------------------------------------------------------------------------------------------------

/*
 * TODO: SDS. For more complicated cases, it may well be woth changing utilised_ into
 *       writeOnDestruct_, and then having the JournalRecord write itself.
 *
 * --> This will require passing in the Journal object
 * --> Will require some careful consideration of locking for the journal.
 */

JournalRecord::JournalRecord(Journal &journal, RecordType type) :
    journal_(journal),
    utilised_(false) {

    if (type != JournalRecord::Uninitialised) {

        initialise(type);
    }
}


JournalRecord::~JournalRecord() {
}


/// Initialise a (new) Journal record, such that it will be valid for writing
/// (once payload data has been added as appropriate).
void JournalRecord::initialise(RecordType type) {

    eckit::zero(head_);
    head_.tag_ = type;
    head_.tagVersion_ = JournalRecord::CurrentTagVersion;
    head_.numEntries_ = 0;

    SYSCALL(::gettimeofday(&head_.timestamp_, NULL));

    marker_ = JournalRecord::TerminationMarker;
}

void JournalRecord::addWriteEntry(const void *data, const Length &length)
{
    // Ensure that the JournalEntry has a copy of the data. Note that this may
    // already have been done by another DataSink (in which case this is a NOP).
    addData(data, length);

    // Add the entry here. By default there is no additional (DataSink-specific)
    // information, so the payload length is zero
    addJournalEntry(JournalRecord::JournalEntry::Write);
}


/// Write the journal record, consisting of three parts:
/// i)   The header
/// ii)  The JournalEntries
/// iii) The end-of-record marker
void JournalRecord::writeRecord(DataHandle& handle) {

    handle.write(&head_, sizeof(head_));

    ASSERT(size_t(head_.numEntries_) == entries_.size());
    Log::info() << "Writing record." << std::endl;
    Log::info() << "nEntries: " << head_.numEntries_ << std::endl;
    Log::info() << "type: " << JournalRecord::RecordTypeNames[head_.tag_] << std::endl;

    for (std::list<JournalEntry>::const_iterator it = entries_.begin(); it != entries_.end(); ++it) {
        handle.write(&it->head_, sizeof(it->head_));

        // If there is data associated with the journal entry then it should be appended below
        // the header information.
        if (it->data_) {
            ASSERT(it->head_.payload_length_ == it->data_->size());
            handle.write(*(it->data_), it->data_->size());
        } else {
            ASSERT(it->head_.payload_length_ == 0);
        }
    }
    
    handle.write(&marker_, sizeof(marker_));
}


void JournalRecord::addData(const void * data, const Length& length) {

    // n.b. The data must be the first thing added to the Journal Record
    if (entries_.empty()) {

        entries_.push_back(JournalEntry());

        JournalEntry& entry = entries_.back();

        eckit::zero(entry.head_);
        entry.head_.tag_ = JournalEntry::Data;
        entry.head_.payload_length_ = length;
        SYSCALL(::gettimeofday(&entry.head_.timestamp_, NULL));

        // n.b. Don't worry about const cast. That is just to make Buffer constructor happy.
        //      The overall Buffer is const...
        //
        // We are making the promise here that the data will outlive the journal writing
        // process.
        entry.data_.reset(new SharableBuffer(const_cast<void*>(data), length, false));

        // Now that something has been added, we should certainly write this entry on exit!
        utilised_ = true;
        head_.numEntries_++;

    } else {
        // We don't need to duplicate the data if multiple DataSinks are reporting to the journal
        ASSERT(utilised_);
        ASSERT(entries_.front().head_.tag_ == JournalEntry::Data);
        ASSERT(entries_.front().head_.payload_length_ == uint64_t(length));
    }
}


void JournalRecord::addJournalEntry(JournalRecord::JournalEntry::EntryType type) {

    // Before we add a journal entry, the data MUST have been added already
    ASSERT(utilised_);
    ASSERT(entries_.front().head_.tag_ == JournalEntry::Data);

    // These are (currently) default journal entries with no attached data.
    // This needs to be extended.
    entries_.push_back(JournalEntry());
    JournalEntry& entry = entries_.back();

    eckit::zero(entry.head_);
    entry.head_.tag_ = type;
    entry.head_.payload_length_ = 0;
    SYSCALL(::gettimeofday(&entry.head_.timestamp_, NULL));

    // Add an entry!
    head_.numEntries_++;
}

// -------------------------------------------------------------------------------------------------

} // namespace multio
