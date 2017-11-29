#include <Steer/O2MCApplication.h>
#include <FairMQChannel.h>
#include <FairMQMessage.h>
#include <FairMQDevice.h>

namespace o2 {
namespace steer {

  template<typename T> TypedVectorSender(const char* name, FairMQChannel &channel) {
    static auto mgr = FairRootManager::Instance();
    auto vector = mgr->InitObjectAs<const std::vector<T>*>(name);
    if (vector) {
      auto buffer = (char*)&(*vector)[0];
      auto buffersize = vector->size()*sizeof(T);
      FairMQMessagePtr message(mOutChannel->NewMessage(buffer, buffersize,
						     [](void* data, void* hint) { }, buffer));
      channel.Send(message);
    }
  }

  // sends a branch as a tmessage (if we can't use a direct vector copy)
  template<typename T> TMessageBranchSender(const char* name, FairMQChannel &channel) {
    static auto mgr = FairRootManager::Instance();
    auto vector = mgr->InitObjectAs<const std::vector<T>*>(name);
    if (vector) {
      auto buffer = (char*)&(*vector)[0];
      auto buffersize = vector->size()*sizeof(T);
      FairMQMessagePtr message(mOutChannel->NewMessage(buffer, buffersize,
						     [](void* data, void* hint) { }, buffer));
      channel.Send(message);
    }
  }

  
  void O2MCApplication::SendData() {
    // Question do we delegate to detectors?
    // ... or iterate over FairRootMgr branch addresses

    // Send meta information: can we still send this atomically with the data??

    //
    for(detectors : activeDetec) {
      detectors->SendData(channel);
    }
    

    // Send primaries + secondaries produced
    TypedVectorSender<o2::MCTrack>("MCTrack", *mOutChannel);
    
    // HOW TO LOOP OVER THE DETECTOR PRODUCTS -- it would be nice having a template vector
    // CAN WE SEND IN PARALLEL ?
    TypedVectorSender<o2::its::MC>("ITSHits", *mITSHitChannel);
   

    for(detID : allIDS) {
      TypededVectorSender<detID::HitType>(detID::BranchName);
    }
    
  }

}
}
