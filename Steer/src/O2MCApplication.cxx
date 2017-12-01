#include <Steer/O2MCApplication.h>
#include <FairMQChannel.h>
#include <FairMQMessage.h>
#include <FairMQDevice.h>
#include <ITSMFTSimulation/Hit.h>
#include <SimulationDataFormat/PrimaryChunk.h>

namespace o2 {
namespace steer {

  template<typename T>
  void TypedVectorSender(const char* name, FairMQChannel& channel, const o2::Data::SubEventInfo& info) {
    static auto mgr = FairRootManager::Instance();
    auto vector = mgr->InitObjectAs<const std::vector<T>*>(name);

    o2::Data::addTrailingObjectToVector(
					*(const_cast<std::vector<T>*>(vector)), info);
    
    if (vector) {
      auto buffer = (char*)&(*vector)[0];
      auto buffersize = vector->size()*sizeof(T);
      FairMQMessagePtr message(channel.NewMessage(buffer, buffersize,
						     [](void* data, void* hint) { }, buffer));
      channel.Send(message);
    }
  }

  
  void O2MCApplication::SendData() {
    // Question do we delegate to detectors?
    // ... or iterate over FairRootMgr branch addresses

    // Send meta information: can we still send this atomically with the data??

    //

    // Send primaries + secondaries produced
    TypedVectorSender<o2::MCTrack>("MCTrack", *mMCTrackChannel, mSubEventInfo);
    
    TypedVectorSender<o2::ITSMFT::Hit>("ITSHit", *mITSChannel, mSubEventInfo);
    // HOW TO LOOP OVER THE DETECTOR PRODUCTS -- it would be nice having a template vector
    // CAN WE SEND IN PARALLEL ?
  }

}
}
