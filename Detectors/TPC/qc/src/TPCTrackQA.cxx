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
  mHist1D.emplace_back("hPhi","; #phi (rad); counts",500,-250,250);	              //| mHist1D[3]
  mHist1D.emplace_back("hTgl","; tan#lambda; counts",500,-250,250);		            //| mHist1D[4]
  mHist1D.emplace_back("hSnp","; sin p; counts",500,-250,250);			              //| mHist1D[5]

  mHist2D.emplace_back("hdedxVsPhi","; #phi (rad); dedx (a.u.)", 180,-M_PI,M_PI,300,0,300);     //| mHist2D[0]
  mHist2D.emplace_back("hdedxVsTgl","; tan#lambda; dedx (a.u.)", 60,-2,2,300,0,300);            //| mHist2D[1]
  mHist2D.emplace_back("hdedxVsncls","; ncls; dedx (a.u.)", 80,0,160,300,0,300);                //| mHist2D[2]
  mHist2D.emplace_back("hdedxVsp","; p (G#it(e)V/#it(c)); dedx (a.u.)", 30,0.1,10,300,0,300);   //| mHist2D[3]
  //mHist2D.emplace_back("hdedxVsphiMIPA","; #phi (rad); dedx (a.u.)", 180,-M_PI,M_PI,25,35,60);  //| mHist2D[4]
  //mHist2D.emplace_back("hdedxVsphiMIPC","; #phi (rad); dedx (a.u.)", 180,-M_PI,M_PI,25,35,60);  //| mHist2D[5]

  mHist1D[0].GetYaxis()->SetTitleOffset(0.9);
  //mHist1D[0].SetDrawOption("P");
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

//______________________________________________________________________________
float TPCTrackQA::getTruncatedMean(std::vector<float> clqVec, float trunclow, float trunchigh)
{
  trunclow = clqVec.size() * trunclow/128;
  trunchigh = clqVec.size() * trunchigh/128;
  if (trunclow >= trunchigh) {
    return (0.);
  }
  std::sort(clqVec.begin(), clqVec.end());
  float mean = 0;
  for (int i = trunclow; i < trunchigh; i++) {
    mean += clqVec.at(i);
  }
  return (mean / (trunchigh - trunclow));
}

//______________________________________________________________________________
void TPCTrackQA::correctCharge(float qtot, float qmax, float trackSnp, float trackTgl, float padHeight, float padWidth) 
{
  float snp2 = trackSnp * trackSnp;
  float factor = std::sqrt((1 - snp2) / (1 + trackTgl * trackTgl));factor /= padHeight;
  qtot *= factor;
  qmax *= factor / padWidth;
  return;
}

// How can I read the track and the cluster file "event by event" to have the references to the proper clusters? 
// In the macro a o2::tpc::ClusterNativeHelper::Reader is used which reads the cluster file event by event simultaneously to the event loop


