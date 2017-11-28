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
#include "TMessage.h"
#include <SimulationDataFormat/Stack.h>
#include <SimulationDataFormat/PrimaryChunk.h>

namespace o2 {
namespace devices {

class TMessageWrapper : public TMessage
{
 public:
  TMessageWrapper(void* buf, Int_t len) : TMessage(buf, len) { ResetBit(kIsOwner); }
  ~TMessageWrapper() override = default;
};

class O2HitMerger : public FairMQDevice
{
 public:
  /// Default constructor
  O2HitMerger() {
	OnData("simdata", &O2HitMerger::HandleData);
  }

  /// Default destructor
  ~O2HitMerger() final = default;

 protected:
  /// Overloads the InitTask() method of FairMQDevice
  void InitTask() final
  {
  }

  /// Overloads the ConditionalRun() method of FairMQDevice
  bool HandleData(FairMQMessagePtr& request, int /*index*/)
  {
    LOG(INFO) << "RECEIVED SIM DATA " << FairLogger::endl;
	return true;
  }
};

} // namespace devices
} // namespace o2

#endif
