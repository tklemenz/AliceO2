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
/// @file   boostPID.h
/// @author Thomas Klemenz, thomas.klemenz@tum.de
///

#ifndef AliceO2_TPC_QC_boostPID_H
#define AliceO2_TPC_QC_boostPID_H

#include <vector>
#include <algorithm>           // std::for_each
#include <boost/format.hpp>    // only needed for printing
#include <boost/histogram.hpp> // make_histogram, regular, weight, indexed
#include <cassert>             // assert
#include <functional>          // std::ref
#include <iostream>            // std::cout, std::cout, std::flush
#include <sstream>             // std::ostringstream

//root includes
#include "TH1F.h"
#include "TH2F.h"

//o2 includes
#include "DataFormatsTPC/Defs.h"

namespace o2
{
namespace tpc
{

class TrackTPC;

namespace qc
{

/// @brief  PID quality control class
///
/// This class is used to extract PID related variables
/// from TrackTPC objects and store it in histograms.
///
/// origin: TPC
/// @author Thomas Klemenz, thomas.klemenz@tum.de
class boostPID
{
 public:
  /// default constructor
  boostPID() = default;

  /// bool extracts intormation from track and fills it to histograms
  /// @return true if information can be extracted and filled to histograms
  bool processTrack(const o2::tpc::TrackTPC& track);

  /// Initialize all histograms
  void initializeHistograms();

  /// Reset all histograms
  void resetHistograms();

  /// Dump results to a file
  void dumpToFile(std::string filename);

  /// get 1D histograms
  std::vector<TH1F>& getHistograms1D() { return mHist1D; }
  const std::vector<TH1F>& getHistograms1D() const { return mHist1D; }

  /// get 2D histograms
  std::vector<TH2F>& getHistograms2D() { return mHist2D; }
  const std::vector<TH2F>& getHistograms2D() const { return mHist2D; }

  void testBoostHisto();

 private:
  std::vector<TH1F> mHist1D{};
  std::vector<TH2F> mHist2D{};

  ClassDefNV(boostPID, 1)
};
} // namespace qc
} // namespace tpc
} // namespace o2

#endif
