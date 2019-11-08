// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#define _USE_MATH_DEFINES

#include <cmath>

#include "TStyle.h"

#include "DataFormatsTPC/dEdxInfo.h"
#include "TPCQC/PID.h"
#include "TPCQC/Helpers.h"

ClassImp(o2::tpc::qc::PID);

using namespace o2::tpc::qc;

PID::PID() : mHist1D{},
             mHist2D{}
{
}

//______________________________________________________________________________
void PID::initializeHistograms()
{
  mHist1D.emplace_back("hNClusters", "; # of clusters; counts", 160, 0, 160); //| mHist1D[0]
  mHist1D.emplace_back("hdEdxTot", "; dEdxTot (a.u.); counts", 200, 0, 200);  //| mHist1D[1]
  mHist1D.emplace_back("hdEdxMax", "; dEdxMax (a.u.); counts", 200, 0, 200);  //| mHist1D[2]
  mHist1D.emplace_back("hPhi", "; #phi (rad); counts", 180, -M_PI, M_PI);     //| mHist1D[3]
  mHist1D.emplace_back("hTgl", "; tan#lambda; counts", 60, -2, 2);            //| mHist1D[4]
  mHist1D.emplace_back("hSnp", "; sin p; counts", 60, -2, 2);                 //| mHist1D[5]

  mHist2D.emplace_back("hdEdxVsPhi", "dEdx (a.u.) vs #phi (rad); #phi (rad); dEdx (a.u.)", 180, -M_PI, M_PI, 300, 0, 300);                                            //| mHist2D[0]
  mHist2D.emplace_back("hdEdxVsTgl", "dEdx (a.u.) vs tan#lambda; tan#lambda; dEdx (a.u.)", 60, -2, 2, 300, 0, 300);                                                   //| mHist2D[1]
  mHist2D.emplace_back("hdEdxVsncls", "dEdx (a.u.) vs ncls; ncls; dEdx (a.u.)", 80, 0, 160, 300, 0, 300);                                                             //| mHist2D[2]

  const auto logPtBinning = helpers::makeLogBinning(30,0.1,10);
  if (logPtBinning.size() > 0) {
    mHist2D.emplace_back("hdEdxVsp", "dEdx (a.u.) vs p (G#it{e}V/#it{c}); p (G#it{e}V/#it{c}); dEdx (a.u.)", logPtBinning.size()-1, logPtBinning.data(), 300,0,300);  //| mHist2D[3]
  }
  //mHist2D.emplace_back("hdedxVsphiMIPA","; #phi (rad); dedx (a.u.)", 180,-M_PI,M_PI,25,35,60);  //| mHist2D[4]
  //mHist2D.emplace_back("hdedxVsphiMIPC","; #phi (rad); dedx (a.u.)", 180,-M_PI,M_PI,25,35,60);  //| mHist2D[5]
}

//______________________________________________________________________________
void PID::resetHistograms()
{
  for (auto& hist : mHist1D) {
    hist.Reset();
  }
  for (auto& hist : mHist2D) {
    hist.Reset();
  }
}

//______________________________________________________________________________
bool PID::processTrack(o2::tpc::TrackTPC const& track)
{
  // ===| variables required for cutting and filling |===
  const auto p = track.getP();
  const auto dEdxTot = track.getdEdx().dEdxTotTPC;
  const auto dEdxMax = track.getdEdx().dEdxMaxTPC;
  const auto phi = track.getPhi();
  const auto tgl = track.getTgl();
  const auto snp = track.getSnp();
  const auto nclusters = track.getNClusterReferences();

  // ===| cuts |===
  // hard coded cuts. Should be more configural in future
  //if (std::abs(p - 0.5) > 0.05) {
  //  return false;
  //}

  // ===| histogram filling |===
  mHist1D[0].Fill(nclusters);
  mHist1D[1].Fill(dEdxTot);
  mHist1D[2].Fill(dEdxMax);
  mHist1D[3].Fill(phi);
  mHist1D[4].Fill(tgl);
  mHist1D[5].Fill(snp);

  mHist2D[0].Fill(phi, dEdxTot);
  mHist2D[1].Fill(tgl, dEdxTot);
  mHist2D[2].Fill(nclusters, dEdxTot);
  mHist2D[3].Fill(p, dEdxTot);

  return true;
}

//______________________________________________________________________________
void PID::setNiceStyle()
{
  gStyle->SetPalette(kCividis);

  for (auto& hist : mHist2D) {
    helpers::setStyleHistogram2D(hist);
  }

  for (auto& hist : mHist1D) {
    helpers::setStyleHistogram1D(hist);
  }
}

//______________________________________________________________________________
void PID::dumpToFile(const std::string filename)
{  
  auto f = std::unique_ptr<TFile>(TFile::Open(filename.c_str(), "recreate"));
  f->WriteObject(&mHist1D[0], "nclusters");
  f->WriteObject(&mHist1D[1], "dEdxTot");
  f->WriteObject(&mHist1D[2], "dEdxMax");
  f->WriteObject(&mHist1D[3], "phi");
  f->WriteObject(&mHist1D[4], "tgl");
  f->WriteObject(&mHist1D[5], "snp");
  f->WriteObject(&mHist2D[0], "dEdxVsphi");
  f->WriteObject(&mHist2D[1], "dEdxVstgl");
  f->WriteObject(&mHist2D[2], "dEdxVsnclusters");
  f->WriteObject(&mHist2D[3], "dEdxVsp");
  f->Close();
}
