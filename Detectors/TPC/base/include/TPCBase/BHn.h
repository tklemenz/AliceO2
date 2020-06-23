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
/// @file   BHn.h
/// @author Thomas Klemenz, thomas.klemenz@tum.de
///

#ifndef AliceO2_TPC_BHN_H
#define AliceO2_TPC_BHN_H

#include <vector>
#include <boost/histogram.hpp>
#include <boost/histogram/make_histogram.hpp>

#include <tuple>

#include "FairLogger.h"

class TH1F;
class TH2F;

using namespace boost::histogram;

/// put all axis types you are going to use in the variant
using axis_t = axis::variant<axis::regular<>, axis::integer<>>;
using axes_t = std::vector<axis_t>;
using histogram_t = histogram<axes_t>;

namespace o2
{
namespace tpc
{

/// @brief  BHn quality control class
///
/// n-dimensional boost histogram class
/// origin: TPC
/// @author Thomas Klemenz, thomas.klemenz@tum.de
class BHn
{
 public:
  /// default constructor
  BHn() = default;
  ~BHn() = default;

  BHn(int nAxes, int nBins, float begin, float end);  // for axes with same range and binning
  BHn(int nAxes, std::vector<int>& nBins, std::vector<float>& begin, std::vector<float>& end); // for various range and binning
  BHn(int nAxes, std::vector<int>& nBins, std::vector<float>& begin, std::vector<float>& end, std::vector<std::string>& name); // for various range and binning

  /// get the number of axes
  int getNAxes() { return mHist.rank(); }

  /// get the number of bins for every axis
  void getNBins(std::vector<int>& vec);

  histogram_t& getHisto() { return mHist; }
  void setHisto(histogram_t& histo) { mHist = histo; }

  TH1F* getTH1(int axis);
  TH2F* getTH2(int axis1, int axis2);

 private:
  axes_t mAxes{};
  histogram_t mHist;
  std::vector<int> mNBins{};
  std::vector<float> mBegin{};
  std::vector<float> mEnd{};

  void initialize(int nAxes, int nBins, float begin, float end);  // for axes with same range and binning
  void initialize(int nAxes, std::vector<int>& nBins, std::vector<float>& begin, std::vector<float>& end); // for various range and binning
  void initialize(int nAxes, std::vector<int>& nBins, std::vector<float>& begin, std::vector<float>& end, std::vector<std::string>& name); // for various range and binning

  ClassDefNV(BHn, 1)
};
} // namespace tpc
} // namespace o2

#endif
