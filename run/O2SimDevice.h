// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALICEO2_DEVICES_SIMDEVICE_H_
#define ALICEO2_DEVICES_SIMDEVICE_H_

#include <FairMQDevice.h>
#include <FairLogger.h>
#include "../macro/o2sim.C"
#include "TVirtualMC.h"

namespace o2 {
namespace devices {

class O2SimDevice : public FairMQDevice
{
  public:
    /// Default constructor
    O2SimDevice() = default;

    /// Default destructor
    ~O2SimDevice() final = default;

  protected:
    /// Overloads the InitTask() method of FairMQDevice
    void InitTask() final {
      LOG(INFO) << "Init SIM device " << FairLogger::endl;
      o2sim_init();
    }

    /// Overloads the ConditionalRun() method of FairMQDevice
    bool ConditionalRun() final {
      static int counter=0;
      LOG(INFO) << "Run SIM device " << FairLogger::endl;
      gRandom->SetSeed(1);
      TVirtualMC::GetMC()->ProcessRun(1);
      // need to solve the problem of writing
      return counter++ < 10;
    }

    void PostRun() final {
      LOG(INFO) << "Shutting down " << FairLogger::endl;
    }

  private:
    std::string mInChannelName = "";
    std::string mOutChannelName = "";
};

} // namespace devices
} // namespace o2

#endif
