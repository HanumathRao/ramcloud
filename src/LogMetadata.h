/* Copyright (c) 2012 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef RAMCLOUD_LOGMETADATA_H
#define RAMCLOUD_LOGMETADATA_H

#include "Common.h"
#include "Crc32C.h"
#include "Segment.h"

namespace RAMCloud {

/**
 * Each segment's first entry is a header that contains vital metadata such
 * as the log the segment is a part of, the segment's identifier within that
 * log, and so on. The header is written automatically upon construction.
 */
struct SegmentHeader {
    SegmentHeader(uint64_t logId,
                  uint64_t segmentId,
                  uint32_t capacity,
                  uint64_t headSegmentIdDuringCleaning)
        : logId(logId),
          segmentId(segmentId),
          capacity(capacity),
          headSegmentIdDuringCleaning(headSegmentIdDuringCleaning),
          checksum()
    {
        Crc32C segmentChecksum;
        segmentChecksum.update(this, sizeof(*this) - sizeof(Crc32C::ResultType));
        checksum = segmentChecksum.getResult();
    }

    /**
     * Return true if the segment this header describes was created by the
     * cleaner. Otherwise, it is or was a log head.
     */
    bool
    generatedByCleaner() const
    {
        return headSegmentIdDuringCleaning != Segment::INVALID_SEGMENT_ID;
    }

    /// ID of the Log this segment belongs to.
    uint64_t logId;

    /// Log-unique ID for this Segment.
    uint64_t segmentId;

    /// Total capacity of this segment in bytes.
    uint32_t capacity;

    /// If != INVALID_SEGMENT_ID, this Segment was created by the cleaner and
    /// the value here is the id of the head segment at the time cleaning
    /// started. Any data in this segment is guaranteed to have come from
    /// earlier segments. This allows us to logically order cleaner-generated
    /// survivor segments, even though they may have IDs larger than head
    /// segments containing newer data.
    uint64_t headSegmentIdDuringCleaning;

    /// Checksum cover all of the above fields in the SegmentHeader.
    Crc32C::ResultType checksum;
} __attribute__((__packed__));
static_assert(sizeof(SegmentHeader) == 32,
              "Header has unexpected padding");

} // namespace RAMCloud

#endif // RAMCLOUD_LOGMETADATA_H
