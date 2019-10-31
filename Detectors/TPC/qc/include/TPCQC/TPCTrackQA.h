// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// @file   TPCTrackQA.h
/// @author Thomas Klemenz, thomas.klemenz@tum.de
///

#ifndef AliceO2_TPC_TPCTrackQA_H
#define AliceO2_TPC_TPCTrackQA_H

//root includes
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
//o2 includes
#include "DataFormatsTPC/Defs.h"
#include "DataFormatsTPC/TrackTPC.h"
#include "TPCBase/Sector.h"

namespace o2
{
namespace tpc
{
namespace qc
{

/// Keep QC information for PID related observables
///
/// This is just a dummy implementation to get started with QC
/// @author Thomas Klemenz thomas.klemenz@tum.de
class TPCTrackQA
{
 public:
  TPCTrackQA();

  static TPCTrackQA& instance()
  {
    static TPCTrackQA tpcTrackQA;
    return tpcTrackQA;
  }

  bool processTrack(o2::tpc::TrackTPC const& track);

  /// Initialize all histograms
  void initializeHistograms();

  /// Reset all histograms
  void resetHistograms();

  /// Set nice style for histograms
  void styleHistogram1D(TH1* histo);

  /// Dump results to a file
  void dumpToFile(const std::string filename);

  // calculate the truncated mean of a vector of cluster charges clqVec
  float getTruncatedMean(std::vector<float> clqVec, float trunclow, float trunchigh);

  // correct cluster charge for track angles and pad size
  void correctCharge(float qtot, float qmax, float trackSnp, float trackTgl, float padHeight, float padWidth);

  std::vector<TH1F>& getHistograms1D() { return mHist1D; }
  const std::vector<TH1F>& getHistograms1D() const { return mHist1D; }

  std::vector<TH2F>& getHistograms2D() { return mHist2D; }
  const std::vector<TH2F>& getHistograms2D() const { return mHist2D; }

 private:
  std::vector<TH1F> mHist1D;
  std::vector<TH2F> mHist2D;

  ClassDefNV(TPCTrackQA, 1)
};
} // namespace qc
} // namespace tpc
} // namespace o2

#endif
