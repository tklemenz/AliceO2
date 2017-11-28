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

#include <memory>
#include "FairMQMessage.h"
#include <FairMQDevice.h>
#include <FairLogger.h>
#include "../macro/o2sim.C"
#include "TVirtualMC.h"
#include "TMessage.h"
#include <SimulationDataFormat/Stack.h>


namespace o2 {
namespace devices {

class TMessageWrapper : public TMessage
{
 public:
  TMessageWrapper(void* buf, Int_t len) : TMessage(buf, len) { ResetBit(kIsOwner); }
  ~TMessageWrapper() override = default;
};

  
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
      long long c=0xDEADBEEF;
      FairMQMessagePtr request(NewSimpleMessage(&c));
      FairMQMessagePtr reply(NewMessage());

      // we get the O2MCApplication and inject the primaries before each run
      auto app = static_cast<o2::steer::O2MCApplication*>(TVirtualMCApplication::Instance());
      
      LOG(INFO) << "Trying to Send  " << FairLogger::endl;
      if( Send(request, "primary-get") >= 0 ) {
	LOG(INFO) << "Waiting for answer " << FairLogger::endl;
    	// asking for primary generation
	if ( Receive(reply, "primary-get") > 0 ) {
          LOG(INFO) << "Answer received, containing " << reply->GetSize() << " bytes " <<  FairLogger::endl;

	  // wrap incoming bytes as a TMessageWrapper which offers "adoption" of a buffer
          auto message = new TMessageWrapper(reply->GetData(), reply->GetSize());
	  LOG(INFO) << "message class " << message->GetClass() << FairLogger::endl;
   	  auto decodedmessage = message->ReadObject(message->GetClass());

	  if (auto st = dynamic_cast<o2::Data::Stack*>(decodedmessage)) {
            app->setPrimaries(st->getPrimaries());
            LOG(INFO) << "Processing " << st->getPrimaries().size() <<  FairLogger::endl;
	    gRandom->SetSeed(extractedSeed);
            TVirtualMC::GetMC()->ProcessRun(1);
	  }
	}
      }
      // need to solve the problem of writing
      return true;
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
