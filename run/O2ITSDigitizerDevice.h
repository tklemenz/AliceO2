// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALICEO2_DEVICES_ITSDIGITIZER_H_
#define ALICEO2_DEVICES_ITSDIGITIZER_H_

#include <memory>
#include "FairMQMessage.h"
#include <FairMQDevice.h>
#include <FairLogger.h>
#include "TVirtualMC.h"
#include <SimulationDataFormat/Stack.h>
#include <SimulationDataFormat/PrimaryChunk.h>
#include <gsl/gsl>
#include "TFile.h"
#include "TTree.h"
#include <TMessage.h>
#include <FairMQParts.h>
#include <ctime>
#include <TStopwatch.h>
#include <TGeoManager.h>
#include <sstream>
#include <cassert>
#include "FairSystemInfo.h"

#include "ITSMFTSimulation/DigiParams.h"
#include "ITSMFTSimulation/Digitizer.h"
#include "ITSMFTSimulation/Hit.h"
#include "ITSBase/GeometryTGeo.h"

namespace o2 {
namespace devices {

class O2ITSDigitizerDevice : public FairMQDevice
{
 public:
  /// Default constructor
  O2ITSDigitizerDevice() {
    // ideally we have a specialized handler function per data channel
    OnData("itsdigitizerchannel", &O2ITSDigitizerDevice::HandleHits);
    mTimer.Start();
  }

  /// Default destructor
  ~O2ITSDigitizerDevice() {
    FairSystemInfo sysinfo;
    LOG(INFO) << "TIME-STAMP " << mTimer.RealTime() << "\t";
    mTimer.Continue();
    LOG(INFO) << "MEM-STAMP " << sysinfo.GetCurrentMemory()/(1024.*1024) << " " << sysinfo.GetMaxMemory() << " MB\n";

    mOutTree.get()->Fill();
    mOutTree.get()->Write();
    mOutFile.get()->Close();
  }

 private:
  /// Overloads the InitTask() method of FairMQDevice
  void InitTask() final
  {
    // load the geometry
    TGeoManager::Import("O2geometry_digi.root");
    
    using o2::ITSMFT::DigiParams;
    // set up the digitizer
    DigiParams param; // RS: TODO: Eventually load this from the CCDB
    param.setContinuous(mContinuous);
    param.setHitDigitsMethod(mUseAlpideSim ? DigiParams::p2dCShape : DigiParams::p2dSimple);
    mDigitizer.setDigiParams(param);

    mDigitizer.setCoeffToNanoSecond(1);
  
    auto geom = new o2::ITS::GeometryTGeo(true);//::Instance();
    geom->fillMatrixCache( Base::Utils::bit2Mask(Base::TransformType::L2G) ); // make sure L2G matrices are loaded
    mDigitizer.setGeometry(geom);
    mDigitizer.init();

    mOutFile = std::make_unique<TFile>("ITSDigits.root", "RECREATE");
    mOutTree = std::make_unique<TTree>("ITSDigits","");
    // register the branch
    mOutTree->Branch("digits", &mDigitsArray);
  }

  template<typename T>
  static gsl::span<T> getView(FairMQMessagePtr& data) {
    return gsl::span<T>(static_cast<T*>(data->GetData()), data->GetSize()/sizeof(T));
  }

  bool HandleHits(FairMQParts& data, int /*index*/)
  {
    LOG(INFO) << "GOT HITS ... digitize them\n";

    // retrieve meta information
    auto infomessage = std::move(data.At(0));
    o2::Data::SubEventInfo info;
    memcpy((void*)&info, infomessage->GetData(), infomessage->GetSize());
    
    auto payloadmessage = std::move(data.At(1));
    gsl::span<ITSMFT::Hit> hitview = getView<ITSMFT::Hit>(payloadmessage);
    LOG(INFO) << " EVENTTIME " << info.eventtime << " #HITS " << hitview.size() << "\n";
    hitview[hitview.size()-1].Print(nullptr);
     
    // decode the time and some other meta information ... and digitize
    mDigitizer.setEventTime(info.eventtime);
    mDigitizer.setCurrSrcID(0);
    mDigitizer.setCurrEvID(info.eventID);

    // MC information
    mDigitizer.process(&hitview, &mDigitsArray);

    // here we could mark hits as done and deallocate them ( in a shared memory setting )
    
    FairSystemInfo sysinfo;
    LOG(INFO) << "TIME-STAMP " << mTimer.RealTime() << "\t";
    mTimer.Continue();
    LOG(INFO) << "MEM-STAMP " << sysinfo.GetCurrentMemory()/(1024.*1024) << " " << sysinfo.GetMaxMemory() << " MB\n";
    return true;
  }

  TStopwatch mTimer;
  ITSMFT::Digitizer mDigitizer;                 ///< Digitizer
  bool mContinuous = true;
  bool mUseAlpideSim = false;
  std::vector<o2::ITSMFT::Digit> mDigitsArray; ///< Container storing the digits
  std::unique_ptr<TFile> mOutFile; // the output file
  std::unique_ptr<TTree> mOutTree; // the output tree for the digits
};

} // namespace devices
} // namespace o2

#endif
