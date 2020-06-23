// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "TPCBase/BHn.h"

//ClassImp(o2::tpc::BHn);

using namespace o2::tpc;
using namespace boost::histogram;

BHn::BHn(int nAxes, int nBins, float begin, float end)
{
  initialize(nAxes, nBins, begin, end);
}

BHn::BHn(int nAxes, std::vector<int> nBins, std::vector<float> begin, std::vector<float> end)
{
  initialize(nAxes, nBins, begin, end);
}

BHn::BHn(int nAxes, std::vector<int> nBins, std::vector<float> begin, std::vector<float> end, std::vector<std::string> name)
{
  initialize(nAxes, nBins, begin, end, name);
}

//______________________________________________________________________________
/*void BHn::makeHistogram()
{
  mHist = histogram(axis::regular<>(2, 0.0, 2.0, "x"), axis::regular<>(2, 0.0, 2.0, "y"));//, axis::regular<>(6, -1.0, 2.0, "z"));
}*/

//______________________________________________________________________________
void BHn::initialize(int nAxes, int nBins, float begin, float end)
{
  for (int i=0; i<nAxes; i++) {
    mAxes.emplace_back(axis::regular<>(nBins, begin, end));
  }

  mHist = histogram(mAxes);
}

//______________________________________________________________________________
void BHn::initialize(int nAxes, std::vector<int> nBins, std::vector<float> begin, std::vector<float> end)   /// maybe do reference to the vectors
{
  if (!((nAxes == nBins.size()) && (nAxes == begin.size()) && (nAxes == end.size()))) { 
    LOG(ERROR) << "Number of bins, begin, and end need to be given for every axis!";
    return;
  }

  for (int i=0; i<nAxes; i++) {
    mAxes.emplace_back(axis::regular<>(nBins.at(i), begin.at(i), end.at(i)));
  }

  mHist = histogram(mAxes);
}

//______________________________________________________________________________ /// maybe do reference to the vectors
void BHn::initialize(int nAxes, std::vector<int> nBins, std::vector<float> begin, std::vector<float> end, std::vector<std::string> name)
{
  if (!((nAxes == nBins.size()) && (nAxes == begin.size()) && (nAxes == end.size() && (nAxes == name.size())))) { 
    LOG(ERROR) << "Number of bins, begin, and end need to be given for every axis!";
    return;
  }

  for (int i=0; i<nAxes; i++) {
    mAxes.emplace_back(axis::regular<>(nBins.at(i), begin.at(i), end.at(i), name.at(i)));
  }

  mHist = make_histogram(mAxes);
}

//______________________________________________________________________________
void BHn::getNBins(std::vector<int>& vec)
{
  for (int i=0; i<mAxes.size(); i++) {
    vec.emplace_back(mAxes.at(i).size());
  }
}