// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file Digitizer.cxx
/// \brief Implementation of the ITS digitizer

#include "SimulationDataFormat/MCCompLabel.h"
#include "ITSMFTBase/Digit.h"
#include "ITSMFTSimulation/Hit.h"
#include "ITSMFTSimulation/Digitizer.h"
#include "MathUtils/Cartesian3D.h"

#include "FairLogger.h"   // for LOG
#include <TRandom.h>
#include <climits>

ClassImp(o2::ITSMFT::Digitizer)

using o2::ITSMFT::Hit;
using o2::ITSMFT::Chip;
using o2::ITSMFT::Digit;

using namespace o2::ITSMFT;
using namespace o2::Base;


//_______________________________________________________________________
void Digitizer::init()
{

  const Int_t numOfChips = mGeometry->getNumberOfChips();

  if (mParams.getHit2DigitsMethod() == DigiParams::p2dCShape && !mParams.getAlpSimResponse()) {
    mAlpSimResp = std::make_unique<o2::ITSMFT::AlpideSimResponse>();
    mAlpSimResp->initData();
    mParams.setAlpSimResponse(mAlpSimResp.get());
  }
    
  for (Int_t i = 0; i < numOfChips; i++) {
    mSimulations.emplace_back(&mParams, i, &mGeometry->getMatrixL2G(i));
  }
}



//_______________________________________________________________________
void Digitizer::setEventTime(double t)
{
  // assign event time, it should be in a strictly increasing order
  // convert to ns
  t *= mCoeffToNanoSecond;

  if (t<mEventTime && mParams.isContinuous()) {
    LOG(FATAL) << "New event time (" << t << ") is < previous event time (" << mEventTime << ")" << FairLogger::endl;
  }
  mEventTime = t;
  // to limit the range of RO Frame IDs we subtract the meaningless offset
  if (mParams.isContinuous()) { // in continuous mode we set the offset only in the very beginning
    if (!mParams.isTimeOffsetSet()) { // offset is initially at -inf
      mParams.setTimeOffset(mEventTime + mParams.getROFrameLenght()*(gRandom->Rndm()-0.5));
    }
  }
  else { // in the triggered mode we start from 0 ROFrame in every event
    mParams.setTimeOffset( mEventTime + mParams.getROFrameLenght()*(gRandom->Rndm()-0.5));
    mROFrameMin = 0;  // so we reset the frame counters
    mROFrameMax = 0;
  }
}

//_______________________________________________________________________
void Digitizer::fillOutputContainer(std::vector<Digit>* digits, UInt_t maxFrame)
{
  // fill output with digits ready to be stored, generating the noise beforehand
  if (maxFrame>mROFrameMax) maxFrame = mROFrameMax;

  LOG(INFO) << "Filling ITS digits output for RO frames " << mROFrameMin << ":" << maxFrame << FairLogger::endl ;

  for (auto &simulation : mSimulations) {
    // add the random noise to all ROFrame being stored
    simulation.addNoise(mROFrameMin,maxFrame);
  }

  // we have to write chips in RO increasing order, therefore have to loop over the frames here
  for (auto rof=mROFrameMin;rof<=maxFrame;rof++) {
    for (auto &simulation : mSimulations) {
      simulation.fillOutputContainer(digits,rof);
    }
  }
  mROFrameMin = maxFrame+1;
}

//_______________________________________________________________________
void Digitizer::setCurrSrcID(int v)
{
  // set current MC source ID
  if ( v > MCCompLabel::maxSourceID() ) {
    LOG(FATAL) << "MC source id " << v << " exceeds max storable in the label "
	       << MCCompLabel::maxSourceID() << FairLogger::endl;
  }
  mCurrSrcID = v;
}

//_______________________________________________________________________
void Digitizer::setCurrEvID(int v)
{
  // set current MC event ID
  if ( v > MCCompLabel::maxEventID() ) {
    LOG(FATAL) << "MC event id " << v << " exceeds max storable in the label "
	       << MCCompLabel::maxEventID() << FairLogger::endl;
  }
  mCurrEvID = v;
}
