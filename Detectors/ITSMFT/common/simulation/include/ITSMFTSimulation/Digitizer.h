// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file Digitizer.h
/// \brief Definition of the ITS digitizer
#ifndef ALICEO2_ITS_DIGITIZER_H
#define ALICEO2_ITS_DIGITIZER_H

#include <vector>
#include <memory>
#include <climits>

#include "Rtypes.h"  // for Digitizer::Class, Double_t, ClassDef, etc
#include "TObject.h" // for TObject

#include "ITSMFTSimulation/SimulationAlpide.h"
#include "ITSMFTSimulation/AlpideSimResponse.h"
#include "ITSMFTSimulation/DigiParams.h"
#include "ITSMFTBase/GeometryTGeo.h"
#include "ITSMFTSimulation/Hit.h"

namespace o2
{
  namespace ITSMFT
  {
    class Digitizer : public TObject
    {
    public:
      
      Digitizer() = default;
      ~Digitizer() override = default;
      Digitizer(const Digitizer&) = delete;
      Digitizer& operator=(const Digitizer&) = delete;


      void init();

      /// Steer conversion of hits to digits
      template<typename IterableContainer>
      void   process(const IterableContainer* hits, std::vector<Digit>* digits);
      
      void   setEventTime(double t);
      double getEventTime()        const  {return mEventTime;}

      void   setContinuous(bool v) {mParams.setContinuous(v);}
      bool   isContinuous()  const {return mParams.isContinuous();}
      void   fillOutputContainer(std::vector<Digit>* digits, UInt_t maxFrame=0xffffffff);

      void   setDigiParams(const o2::ITSMFT::DigiParams& par) {mParams = par;}
      const  o2::ITSMFT::DigiParams& getDigitParams()   const {return mParams;}

      void   setCoeffToNanoSecond(double cf)                  { mCoeffToNanoSecond = cf; }
      double getCoeffToNanoSecond()                     const { return mCoeffToNanoSecond; }

      int getCurrSrcID() const { return mCurrSrcID; }
      int getCurrEvID()  const { return mCurrEvID; }

      void setCurrSrcID(int v);
      void setCurrEvID(int v);
            
      // provide the common ITSMFT::GeometryTGeo to access matrices and segmentation
      void setGeometry(const o2::ITSMFT::GeometryTGeo* gm) { mGeometry = gm;}
      
    private:

      const o2::ITSMFT::GeometryTGeo* mGeometry = nullptr;    ///< ITS OR MFT upgrade geometry
      std::vector<o2::ITSMFT::SimulationAlpide> mSimulations; ///< Array of chips response simulations
      o2::ITSMFT::DigiParams mParams;            ///< digitization parameters
      double mEventTime = 0;                     ///< global event time
      double mCoeffToNanoSecond = 1.0;           ///< coefficient to convert event time (Fair) to ns
      bool   mContinuous = false;                ///< flag for continuous simulation
      UInt_t mROFrameMin = 0;                    ///< lowest RO frame of current digits
      UInt_t mROFrameMax = 0;                    ///< highest RO frame of current digits
      int    mCurrSrcID = 0;                     ///< current MC source from the manager
      int    mCurrEvID = 0;                      ///< current event ID from the manager

      std::unique_ptr<o2::ITSMFT::AlpideSimResponse> mAlpSimResp; // simulated response 
      
      ClassDefOverride(Digitizer, 2);
    };

template<typename IterableContainer>
inline
//_______________________________________________________________________
void Digitizer::process(const IterableContainer* hits, std::vector<Digit>* digits)
{
  // digitize single event
  
  const Int_t numOfChips = mGeometry->getNumberOfChips();  

  // estimate the smalles RO Frame this event may have
  double hTime0 = mEventTime - mParams.getTimeOffset();
  if (hTime0 > UINT_MAX) {
    LOG(WARNING) << "min Hit RO Frame undefined: time: " << hTime0 << " is in far future: "
		 << " EventTime: " << mEventTime << " TimeOffset: "
		 << mParams.getTimeOffset() << FairLogger::endl;
    return;
  }

  if (hTime0<0) hTime0 = 0.;
  UInt_t minNewROFrame = static_cast<UInt_t>(hTime0/mParams.getROFrameLenght());

  LOG(INFO) << "Digitizing ITS event at time " << mEventTime
	    << " (TOffset= " << mParams.getTimeOffset() << " ROFrame= " << minNewROFrame << ")"
	    << " cont.mode: " << isContinuous() << " current Min/Max RO Frames "
	    << mROFrameMin << "/" << mROFrameMax << FairLogger::endl ;
  
  if (mParams.isContinuous() && minNewROFrame>mROFrameMin) {
    // if there are already digits cached for previous RO Frames AND the new event
    // cannot contribute to these digits, move them to the output container
    if (mROFrameMax<minNewROFrame) mROFrameMax = minNewROFrame-1;
    for (auto rof=mROFrameMin; rof<minNewROFrame; rof++) {
      fillOutputContainer(digits, rof);
    }
    //    fillOutputContainer(digits, minNewROFrame-1);
  }
  
  // accumulate hits for every chip
  for(auto& hit : *hits) {
    // RS: ATTENTION: this is just a trick until we clarify how the hits from different source are
    // provided and identified. At the moment we just create a combined identifier from eventID
    // and sourceID and store it TEMPORARILY in the cached Point's TObject UniqueID
    const_cast<Hit&>(hit).SetSrcEvID(mCurrSrcID,mCurrEvID); 
    mSimulations[hit.GetDetectorID()].InsertHit(&hit);
  }
    
  // Convert hits to digits  
  for (auto &simulation : mSimulations) {
    simulation.Hits2Digits(mEventTime, mROFrameMin, mROFrameMax);
    simulation.ClearHits();
  }

  // in the triggered mode store digits after every MC event
  if (!mParams.isContinuous()) {
    fillOutputContainer(digits, mROFrameMax);
  }
}

    
  }
}

#endif /* ALICEO2_ITS_DIGITIZER_H */
