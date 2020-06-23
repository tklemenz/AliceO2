// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include <boost/histogram/algorithm/project.hpp>

#include "TH1F.h"
#include "TH2F.h"

#include "TPCBase/BHn.h"

ClassImp(o2::tpc::BHn);

using namespace o2::tpc;
using namespace boost::histogram;

BHn::BHn(int nAxes, int nBins, float begin, float end)
{
  initialize(nAxes, nBins, begin, end);
  mNBins.emplace_back(nBins);
  mBegin.emplace_back(begin);
  mEnd.emplace_back(end);
}

BHn::BHn(int nBins, float begin, float end)
{
  initialize(1, nBins, begin, end);
  mNBins.emplace_back(nBins);
  mBegin.emplace_back(begin);
  mEnd.emplace_back(end);
}

BHn::BHn(int nBins1, float begin1, float end1, int nBins2, float begin2, float end2)
{
  initialize(nBins1, begin1, end1, nBins2, begin2, end2);
  mNBins.emplace_back(nBins1);
  mNBins.emplace_back(nBins2);
  mBegin.emplace_back(begin1);
  mBegin.emplace_back(begin2);
  mEnd.emplace_back(end1);
  mEnd.emplace_back(end2);
}

BHn::BHn(int nAxes, std::vector<int>& nBins, std::vector<float>& begin, std::vector<float>& end)
{
  initialize(nAxes, nBins, begin, end);
  mNBins = nBins;
  mBegin = begin;
  mEnd = end;
}

BHn::BHn(int nAxes, std::vector<int>& nBins, std::vector<float>& begin, std::vector<float>& end, std::vector<std::string>& name)
{
  initialize(nAxes, nBins, begin, end, name);
  mNBins = nBins;
  mBegin = begin;
  mEnd = end;
}

//______________________________________________________________________________
void BHn::initialize(int nAxes, int nBins, float begin, float end)
{
  for (int i=0; i<nAxes; i++) {
    mAxes.emplace_back(axis::regular<>(nBins, begin, end));
  }

  mHist = histogram(mAxes);
}

//______________________________________________________________________________
void BHn::initialize(int nBins1, float begin1, float end1, int nBins2, float begin2, float end2)
{
  
  mAxes.emplace_back(axis::regular<>(nBins1, begin1, end1));
  mAxes.emplace_back(axis::regular<>(nBins2, begin2, end2));

  mHist = histogram(mAxes);
}

//______________________________________________________________________________
void BHn::initialize(int nAxes, std::vector<int>& nBins, std::vector<float>& begin, std::vector<float>& end)
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

//______________________________________________________________________________
void BHn::initialize(int nAxes, std::vector<int>& nBins, std::vector<float>& begin, std::vector<float>& end, std::vector<std::string>& name)
{
  if (!((nAxes == nBins.size()) && (nAxes == begin.size()) && (nAxes == end.size() && (nAxes == name.size())))) { 
    LOG(ERROR) << "Number of bins, begin, and end need to be given for every axis!";
    return;
  }

  for (int i=0; i<nAxes; i++) {
    mAxes.emplace_back(axis::regular<>(nBins.at(i), begin.at(i), end.at(i), name.at(i)));
  }

  mHist = histogram(mAxes);
}

//______________________________________________________________________________
void BHn::getNBins(std::vector<int>& vec)
{
  for (int i=0; i<mAxes.size(); i++) {
    vec.emplace_back(mAxes.at(i).size());
  }
}

//______________________________________________________________________________
TH1F* BHn::getTH1(int axis, std::string name, std::string axis1Title, std::string axis2Title)  // TODO: check rank of histo to not project if rank==1
{
  std::vector<int> iter{axis}; // TODO: find a better solution for an iterator

  TH1F* h = new TH1F(Form("%s", name.c_str()), Form("; %s; %s", axis1Title.c_str(), axis2Title.c_str()), mNBins[axis], mBegin[axis], mEnd[axis]);

  auto projection = algorithm::project(mHist, iter);

  for (auto&& x : indexed(projection)) {
    h->SetBinContent(x.index(0)+1, *x);
  }

  return h;
}

//______________________________________________________________________________
TH2F* BHn::getTH2(int axis1, int axis2, std::string name, std::string axis1Title, std::string axis2Title, std::string axis3Title)  // TODO: check rank of histo to not project if rank==2
{
  std::vector<int> iter{axis1, axis2}; // TODO: find a better solution for an iterator

  TH2F* h = new TH2F(Form("%s", name.c_str()), Form("; %s; %s; %s", axis1Title.c_str(), axis2Title.c_str(), axis3Title.c_str()), mNBins[axis1], mBegin[axis1], mEnd[axis1], mNBins[axis2], mBegin[axis2], mEnd[axis2]);

  auto projection = algorithm::project(mHist, iter);

  for (auto&& x : indexed(projection)) {
    h->SetBinContent(x.index(0)+1, x.index(1)+1, *x);
  }

  return h;
}
