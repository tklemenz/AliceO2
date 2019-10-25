// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include <cmath>

#include "DataFormatsTPC/dEdxInfo.h"

#include "TPCQC/TPCTrackQA.h"

ClassImp(o2::tpc::qc::TPCTrackQA);

using namespace o2::tpc::qc;

TPCTrackQA::TPCTrackQA() : mHist1D{}
{
}

void TPCTrackQA::initializeHistograms()
{
  mHist1D.emplace_back("hChargeTot","; charge tot; counts",1000,0,1000);          //| mHist1D[0]
  mHist1D.emplace_back("hChargeMax","; charge max; counts",500,0,500);            //| mHist1D[1]
  mHist1D.emplace_back("hNClusters","; # of clusters; counts",160,0,160);         //| mHist1D[2]
  mHist1D.emplace_back("hTime","; time; counts",10000,0,100000);                  //| mHist1D[3]
  mHist1D.emplace_back("hSigmaTime","; sigma time; counts",40,0,2);               //| mHist1D[4]
  mHist1D.emplace_back("hPad","; time; counts",1400,0,140);                       //| mHist1D[5]
  mHist1D.emplace_back("hSigmaPad","; sigma time; counts",60,0,2);                //| mHist1D[6]
  mHist1D.emplace_back("hdEdxTot","; dEdxTot; counts",200,0,200);                 //| mHist1D[7]
  mHist1D.emplace_back("hdEdxMax","; dEdxMax; counts",200,0,200);                 //| mHist1D[8]
}

void TPCTrackQA::resetHistograms()
{
  for (auto& hist : mHist1D) {
    hist.Reset();
  }
}

bool TPCTrackQA::processTrack(o2::tpc::TrackTPC const& track)
{
  // ===| variables required for cutting and filling |===
  const auto p = track.getP();
  const auto dEdx = track.getdEdx().dEdxTotTPC;

  // ===| cuts |===
  // hard coded cuts. Should be more configural in future
  if (std::abs(p - 0.5) > 0.05) {
    return false;
  }

  if (dEdx > 70) {
    return false;
  }

  // ===| histogram filling |===
  mHist1D[2].Fill(track.getNClusterReferences());
  mHist1D[7].Fill(dEdx);


  return true;
}

bool startTrackQA() {
  return true;
}
