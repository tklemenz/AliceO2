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
/// @file   PID.h
/// @author Thomas Klemenz, thomas.klemenz@tum.de
///

#ifndef AliceO2_TPC_PID_H
#define AliceO2_TPC_PID_H

//root includes
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TCanvas.h"
//o2 includes
#include "DataFormatsTPC/Defs.h"
#include "DataFormatsTPC/TrackTPC.h"

namespace o2
{
namespace tpc
{
namespace qc
{

/// Keep QC information for PID related observables
///

class PID
{
 public:
  PID();

  static PID& instance()
  {
    static PID pid;
    return pid;
  }

  bool processTrack(o2::tpc::TrackTPC const& track);

  /// Initialize all histograms
  void initializeHistograms();

  /// Reset all histograms
  void resetHistograms();

  /// Set color palette, OptStat and call setStyleHistogram
  void setNiceStyle();

  /// Dump results to a file
  void dumpToFile(const std::string filename);

  std::vector<TH1F>& getHistograms1D() { return mHist1D; }
  const std::vector<TH1F>& getHistograms1D() const { return mHist1D; }

  std::vector<TH2F>& getHistograms2D() { return mHist2D; }
  const std::vector<TH2F>& getHistograms2D() const { return mHist2D; }

 private:
  std::vector<TH1F> mHist1D;
  std::vector<TH2F> mHist2D;

  ClassDefNV(PID, 1)
};
} // namespace qc
} // namespace tpc
} // namespace o2

#endif
