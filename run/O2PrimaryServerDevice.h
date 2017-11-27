// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef O2_DEVICES_PRIMSERVDEVICE_H_
#define O2_DEVICES_PRIMSERVDEVICE_H_

#include <FairMQDevice.h>
#include <FairLogger.h>
#include <FairGenerator.h>
#include <FairBoxGenerator.h>
#include <FairPrimaryGenerator.h>
#include <FairMQPoller.h>
#include <SimulationDataFormat/Stack.h>
#include <FairMCEventHeader.h>

namespace o2 {
namespace devices {

class O2PrimaryServerDevice : public FairMQDevice
{
  public:
    /// Default constructor
    O2PrimaryServerDevice() = default;

    /// Default destructor
    ~O2PrimaryServerDevice() final = default;

  protected:
    /// Overloads the InitTask() method of FairMQDevice
    void InitTask() final {
      LOG(INFO) << "Init SIM device " << FairLogger::endl;

      auto boxGen = new FairBoxGenerator(211, 10); /*protons*/
      boxGen->SetEtaRange(-0.9, 0.9);
      boxGen->SetPRange(0.1, 5);
      boxGen->SetPhiRange(0., 360.);
      boxGen->SetDebug(kTRUE);
      mPrimGen.AddGenerator(boxGen);
      mPrimGen.SetEvent(&mEventHeader);
    }

    /// Overloads the ConditionalRun() method of FairMQDevice
    bool ConditionalRun() final
    {
      std::unique_ptr<FairMQPoller> poller(
        fTransportFactory->CreatePoller(fChannels, {"primary-get"}));

      poller->Poll(-1);
      if (poller->CheckInput("primary-get", 0)) {
        std::unique_ptr<FairMQMessage> input(fTransportFactory->CreateMessage());

        if (Receive(input, "primary-get") > 0) {
          std::string serialString(static_cast<char*>(input->GetData()), input->GetSize());

          // someone asked for an event

          // do nothing for moment
          mStack.Reset();
          mPrimGen.GenerateEvent(&mStack);
        }
      }
      // run infinitely
      return true;
    }

   private:
    std::string mOutChannelName = "";
    FairPrimaryGenerator mPrimGen;
    FairMCEventHeader mEventHeader;
    o2::Data::Stack mStack; // the stack which is filled
};

} // namespace devices
} // namespace o2

#endif
