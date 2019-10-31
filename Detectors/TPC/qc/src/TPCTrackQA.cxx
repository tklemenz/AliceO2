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

#include "TPCQC/TPCTrackQA.h"

ClassImp(o2::tpc::qc::TPCTrackQA);

using namespace o2::tpc::qc;

TPCTrackQA::TPCTrackQA() : mHist1D{},
                           mHist2D{}
{
}

//______________________________________________________________________________
void TPCTrackQA::initializeHistograms()
{
  mHist1D.emplace_back("hNClusters","; # of clusters; counts",160,0,160);         //| mHist1D[0]
  mHist1D.emplace_back("hdedxTot","; dedxTot (a.u.); counts",200,0,200);          //| mHist1D[1]
  mHist1D.emplace_back("hdedxMax","; dedxMax (a.u.); counts",200,0,200);          //| mHist1D[2]
  mHist1D.emplace_back("hPhi","; #phi (rad); counts",180,-M_PI,M_PI);	          //| mHist1D[3]
  mHist1D.emplace_back("hTgl","; tan#lambda; counts",60,-2,2);		          //| mHist1D[4]
  mHist1D.emplace_back("hSnp","; sin p; counts",60,-2,2);			  //| mHist1D[5]

  mHist2D.emplace_back("hdedxVsPhi","dedx (a.u.) vs #phi (rad); #phi (rad); dedx (a.u.)", 180,-M_PI,M_PI,300,0,300);              //| mHist2D[0]
  mHist2D.emplace_back("hdedxVsTgl","dedx (a.u.) vs tan#lambda; tan#lambda; dedx (a.u.)", 60,-2,2,300,0,300);                     //| mHist2D[1]
  mHist2D.emplace_back("hdedxVsncls","dedx (a.u.) vs ncls; ncls; dedx (a.u.)", 80,0,160,300,0,300);                               //| mHist2D[2]
  mHist2D.emplace_back("hdedxVsp","dedx (a.u.) vs p (G#it{e}V/#it{c}); p (G#it{e}V/#it{c}); dedx (a.u.)", 30,-1,1,300,0,300);     //| mHist2D[3]
  //mHist2D.emplace_back("hdedxVsphiMIPA","; #phi (rad); dedx (a.u.)", 180,-M_PI,M_PI,25,35,60);  //| mHist2D[4]
  //mHist2D.emplace_back("hdedxVsphiMIPC","; #phi (rad); dedx (a.u.)", 180,-M_PI,M_PI,25,35,60);  //| mHist2D[5]

  //mHist1D[0].GetYaxis()->SetTitleOffset(0.9);
}

//______________________________________________________________________________
void TPCTrackQA::resetHistograms()
{
  for (auto& hist : mHist1D) {
    hist.Reset();
  }
  for (auto& hist : mHist2D) {
    hist.Reset();
  }
}

//______________________________________________________________________________
bool TPCTrackQA::processTrack(o2::tpc::TrackTPC const& track)
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

  mHist2D[0].Fill(phi,dEdxTot);
  mHist2D[1].Fill(tgl,dEdxTot);
  mHist2D[2].Fill(nclusters,dEdxTot);
  mHist2D[3].Fill(p,dEdxTot);

  return true;
}

//______________________________________________________________________________
void TPCTrackQA::drawHistograms()
{
  auto* c1 = new TCanvas("c1","PID",1200,600);
  c1->Divide(2,2);
  c1->cd(1);
  mHist2D[0].Draw();
  gPad->SetLogz();
  c1->cd(2);
  mHist2D[1].Draw();
  gPad->SetLogz();
  c1->cd(3);
  mHist2D[2].Draw();
  gPad->SetLogz();
  c1->cd(4);
  TPCTrackQA::binLogX(&mHist2D[3]);
  mHist2D[3].Draw();
  gPad->SetLogz();
  gPad->SetLogx();
}

//______________________________________________________________________________
void TPCTrackQA::binLogX(TH2 *h)
{
  TAxis *axis = h->GetXaxis();
  int bins = axis->GetNbins();

  Axis_t from = axis->GetXmin();
  Axis_t to = axis->GetXmax();
  Axis_t width = (to - from) / bins;
  Axis_t *new_bins = new Axis_t[bins +1];

  for (int i = 0; i<= bins; i++) {
    new_bins[i] = std::pow(10, from + i * width);
  }
  axis->Set(bins,new_bins);
  delete new_bins;
}

//______________________________________________________________________________
void TPCTrackQA::setStyleHistogram2D(TH2 &histo)
{
  histo.SetOption("colz");
  histo.SetMinimum(0.9);
}

//______________________________________________________________________________
void TPCTrackQA::setNiceStyle()
{
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kCividis);

  for (auto& hist : mHist2D) {
    TPCTrackQA::setStyleHistogram2D(hist);
  }
}

//______________________________________________________________________________
void TPCTrackQA::dumpToFile(const std::string filename)
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
