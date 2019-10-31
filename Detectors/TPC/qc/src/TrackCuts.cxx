// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "FairLogger.h"

#include "TPCQC/TrackCuts.h"

ClassImp(o2::tpc::qc::TrackCuts);

using namespace o2::tpc::qc;

TrackCuts::TrackCuts() : mPMin(-999),
                         mPMax(-999),
                         mNClusMin(-999)
{
  LOG(INFO) << "Don't forget to set the cut parameters. They were initialized with the default value -999.";
}

TrackCuts::TrackCuts(float PMin, float PMax, float NClusMin) : mPMin(PMin),
                                                               mPMax(PMax),
                                                               mNClusMin(NClusMin)
{
}

//______________________________________________________________________________
bool TrackCuts::goodTrack(o2::tpc::TrackTPC const& track)
{
  const auto p = track.getP();
  const auto nclusters = track.getNClusterReferences();

  if (p > mPMax) {
    return false;
  }
  if (p < mPMin) {
    return false;
  }
  if (nclusters < mNClusMin) {
    return false;
  }
  return true;
}
