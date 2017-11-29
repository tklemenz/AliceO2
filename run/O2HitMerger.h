// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALICEO2_DEVICES_HITMERGER_H_
#define ALICEO2_DEVICES_HITMERGER_H_

#include <memory>
#include "FairMQMessage.h"
#include <FairMQDevice.h>
#include <FairLogger.h>
#include "../macro/o2sim.C"
#include "TVirtualMC.h"
#include <SimulationDataFormat/Stack.h>
#include <SimulationDataFormat/PrimaryChunk.h>
#include <gsl/gsl>

namespace o2 {
namespace devices {

class O2HitMerger : public FairMQDevice
{
 public:
  /// Default constructor
  O2HitMerger() {
    // ideally we have a specialized handler function per data channel
    OnData("mctracks", &O2HitMerger::HandleMCTrackData);
    OnData("itshits", &O2HitMerger::HandleITSHits);
  }

  /// Default destructor
  ~O2HitMerger() final = default;

 protected:
  /// Overloads the InitTask() method of FairMQDevice
  void InitTask() final
  {
  }

  template<typename T>
  gsl::span<T> getView(FairMQMessagePtr& data) {
    return gsl::span<T>(static_cast<T*>(data->GetData()), data->GetSize()/sizeof(T));
  }

  /// Overloads the ConditionalRun() method of FairMQDevice
  bool HandleMCTrackData(FairMQMessagePtr& data, int /*index*/)
  {
    auto mctracks = getView<o2::MCTrack>(data);

    // calculate number of primaries + secondaries
    int primcounter = 0;
    for (auto& t : mctracks) {
      primcounter += (t.getMotherTrackId() < 0) ? 1 : 0;
    }
    LOG(INFO) << "RECEIVED SIM DATA :" << mctracks.size() << " of which " << primcounter << " are primaries "
              << FairLogger::endl;

    return true;
  }

  /// Overloads the ConditionalRun() method of FairMQDevice
  bool HandleITSHits(FairMQMessagePtr& data, int /*index*/)
  {
    auto itshits = getView<o2::ITSMFT::Hit>(data);
    LOG(INFO) << "RECEIVED ITS DATA :" << itshits.size() << "\n"
              << FairLogger::endl;

    return true;
  }

};

} // namespace devices
} // namespace o2

#endif
