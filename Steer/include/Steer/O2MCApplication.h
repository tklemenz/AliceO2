#ifndef O2_MC_APPLICATION_H
#define O2_MC_APPLICATION_H

#include "FairMCApplication.h"
#include "Rtypes.h"                     // for Int_t, Bool_t, Double_t, etc
#include <iostream>
#include <TParticle.h>
#include <vector>
#include <SimulationDataFormat/Stack.h>
#include <FairRootManager.h>
#include <FairDetector.h>

class FairMQChannel;

namespace o2 {
namespace steer {

// O2 specific changes/overrides to FairMCApplication
// (like for device based processing in which we
//  forward the data instead of using FairRootManager::Fill())
class O2MCApplication : public FairMCApplication
{
  public:
    using FairMCApplication::FairMCApplication;
    ~O2MCApplication() override = default;

    // triggers data sending/io
    void SendData();
    
    /** Define actions at the end of event */
    void  FinishEvent() override {
      // This special finish event version does not fill the output tree of FairRootManager
      // but forwards the data to the HitMerger
      SendData();

      // call end of event on active detectors
      for(auto det : listActiveDetectors) {
    	det->EndOfEvent();
      }
    }
    
    /** Define actions at the end of run */
    void  FinishRun();

    /** Generate primary particles */
    void  GeneratePrimaries() override {
      // ordinarily we would call the event generator ...

      // but here we init the stack from
      // a vector of particles that someone sets externally
      static_cast<o2::Data::Stack*>(GetStack())->initFromPrimaries(mPrimaries);
    }
 
    void setPrimaries(std::vector<TParticle> const &p) {
      mPrimaries = p;
    }

    void setMCTrackChannel(FairMQChannel* channel) { mMCTrackChannel = channel; }
    //void setTPCChannel(int index, FairMQChannel* channel) { mTPCChannels[index] = channel; }
    void setITSChannel(FairMQChannel* channel) { mITSChannel = channel; }
    
    std::vector<TParticle> mPrimaries;
    FairMQChannel*         mMCTrackChannel;
    //FairMQChannel*         mTPCChannels[36]; // avoid hard coded number
    FairMQChannel*         mITSChannel;

    ClassDefOverride(O2MCApplication,4)  //Interface to MonteCarlo application
};

} // end namespace steer
} // end namespace o2

#endif
