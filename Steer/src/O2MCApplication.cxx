#include <Steer/O2MCApplication.h>
#include <FairMQChannel.h>
#include <FairMQMessage.h>
#include <FairMQDevice.h>
#include <ITSMFTSimulation/Hit.h>

namespace o2 {
namespace steer {

  template<typename T>
  void TypedVectorSender(const char* name, FairMQChannel &channel) {
    static auto mgr = FairRootManager::Instance();
    auto vector = mgr->InitObjectAs<const std::vector<T>*>(name);
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
    TypedVectorSender<o2::MCTrack>("MCTrack", *mMCTrackChannel);
    
    TypedVectorSender<o2::ITSMFT::Hit>("ITSHit", *mITSChannel);
    // HOW TO LOOP OVER THE DETECTOR PRODUCTS -- it would be nice having a template vector
    // CAN WE SEND IN PARALLEL ?
  }

}
}
