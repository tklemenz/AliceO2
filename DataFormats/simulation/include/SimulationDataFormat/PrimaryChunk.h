// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALICEO2_DATA_PRIMARYCHUNK_H_
#define ALICEO2_DATA_PRIMARYCHUNK_H_

namespace o2 {
namespace Data {

struct SubEventInfo {
  uint32_t eventID; // which event ID
  uint16_t part;    // which part of the eventID
  uint16_t nparts;  // out of how many parts
  uint32_t seed; // seed for RNG
  uint32_t index;
  ClassDefNV(SubEventInfo, 1);
};

// Encapsulating primaries/tracks
// to be processed by the simulation processors.
// May be part of an event or multiple events. Contains
// seed information
struct PrimaryChunk {
  std::vector<SubEventInfo> mEventIDs;
  std::vector<TParticle> mParticles;
  ClassDefNV(PrimaryChunk, 1);
};

}
}

#endif

