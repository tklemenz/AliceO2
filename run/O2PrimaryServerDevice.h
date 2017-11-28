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
#include <FairMQMessage.h>
#include <SimulationDataFormat/Stack.h>
#include <FairMCEventHeader.h>
#include <TMessage.h>
#include <TClass.h>

namespace o2 {
namespace devices {

class O2PrimaryServerDevice : public FairMQDevice
{
  public:
    /// Default constructor
    O2PrimaryServerDevice() {
      OnData("primary-get", &O2PrimaryServerDevice::HandleRequest);
    }

    /// Default destructor
    ~O2PrimaryServerDevice() final = default;

  protected:
    /// Overloads the InitTask() method of FairMQDevice
    void InitTask() final {
      LOG(INFO) << "Init SIM device " << FairLogger::endl;

      auto boxGen = new FairBoxGenerator(211, 500); /*protons*/
      boxGen->SetEtaRange(-0.9, 0.9);
      boxGen->SetPRange(0.1, 5);
      boxGen->SetPhiRange(0., 360.);
      boxGen->SetDebug(kTRUE);
      mPrimGen.AddGenerator(boxGen);
      mPrimGen.SetEvent(&mEventHeader);
    }

    /// Overloads the ConditionalRun() method of FairMQDevice
    bool HandleRequest(FairMQMessagePtr& request, int /*index*/)
    {
      LOG(INFO) << "Received request for work " << FairLogger::endl;
      
      mStack.Reset();
      mPrimGen.GenerateEvent(&mStack);

      TMessage* tmsg = new TMessage(kMESS_OBJECT);
      //tmsg->WriteObjectAny((void*)&mStack, TClass::GetClass("o2::Data::Stack"));
      tmsg->WriteObject(&mStack);
      auto free_tmessage = [](void* data, void* hint) { delete static_cast<TMessage*>(hint); };
      std::unique_ptr<FairMQMessage> message(
      fTransportFactory->CreateMessage(tmsg->Buffer(), tmsg->BufferSize(), free_tmessage, tmsg));

      long long token = 0xABABABAB;
      FairMQMessagePtr reply(NewSimpleMessage(&token));
      // send answer
      if ( Send(message, "primary-get") > 0 ) {
        LOG(INFO) << "reply send " << FairLogger::endl;
        return true;
      }
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
