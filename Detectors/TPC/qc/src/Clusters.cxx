// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "TPCQC/Clusters.h"
#include "TPCBase/Painter.h"
#include "TPCBase/ROC.h"
#include "TH1.h"
#include "TH2.h"
#include "TPCBase/Mapper.h"
#include "TFile.h"

ClassImp(o2::tpc::qc::Clusters);

using namespace o2::tpc::qc;

//______________________________________________________________________________
bool Clusters::processCluster(o2::tpc::ClusterNative const& cluster, o2::tpc::Sector const sector, const int row)
{
  const int nROC = row < 63 ? int(sector) : int(sector) + 36;
  const int rocRow = row < 63 ? row : row - 63;

  const float pad = cluster.getPad();

  const uint16_t qMax = cluster.qMax;
  const uint16_t qTot = cluster.qTot;
  const float sigmaPad = cluster.getSigmaPad();
  const float sigmaTime = cluster.getSigmaTime();
  const float timeBin = cluster.getTime();

  float count = mNClusters.getCalArray(nROC).getValue(rocRow, pad);
  mNClusters.getCalArray(nROC).setValue(rocRow, pad, count + 1);

  float charge = mQMax.getCalArray(nROC).getValue(rocRow, pad);
  mQMax.getCalArray(nROC).setValue(rocRow, pad, charge + qMax);

  charge = mQTot.getCalArray(nROC).getValue(rocRow, pad);
  mQTot.getCalArray(nROC).setValue(rocRow, pad, charge + qTot);

  count = mSigmaTime.getCalArray(nROC).getValue(rocRow, pad);
  mSigmaTime.getCalArray(nROC).setValue(rocRow, pad, count + sigmaTime);

  count = mSigmaPad.getCalArray(nROC).getValue(rocRow, pad);
  mSigmaPad.getCalArray(nROC).setValue(rocRow, pad, count + sigmaPad);

  count = mTimeBin.getCalArray(nROC).getValue(rocRow, pad);
  mTimeBin.getCalArray(nROC).setValue(rocRow, pad, count + timeBin);

  return true;
}

//______________________________________________________________________________
void Clusters::analyse()
{
  mQMax /= mNClusters;
  mQTot /= mNClusters;
  mSigmaTime /= mNClusters;
  mSigmaPad /= mNClusters;
  mTimeBin /= mNClusters;
}

//______________________________________________________________________________
void Clusters::draw()
{
  o2::tpc::painter::draw(mNClusters);
  o2::tpc::painter::draw(mQMax);
  o2::tpc::painter::draw(mQTot);
  o2::tpc::painter::draw(mSigmaTime);
  o2::tpc::painter::draw(mSigmaPad);
  o2::tpc::painter::draw(mTimeBin);
}

TCanvas* Clusters::paint(const CalDet<float>& calDet)
{
  using DetType = CalDet<float>;
  using CalType = CalArray<float>;

  static const Mapper& mapper = Mapper::instance();

  // ===| name and title |======================================================
  const auto title = calDet.getName().c_str();
  std::string name = calDet.getName();
  std::replace(name.begin(), name.end(), ' ', '_');

  // ===| define histograms |===================================================
  // TODO: auto scaling of ranges based on mean and variance?
  //       for the moment use roots auto scaling

  // set buffer size such that autoscaling uses the full range. This is about 2MB per histogram!
  const int bufferSize = TH1::GetDefaultBufferSize();
  TH1::SetDefaultBufferSize(Sector::MAXSECTOR * mapper.getPadsInSector());

  auto hAside1D = new TH1F(Form("h_Aside_1D_%s", name.c_str()), Form("%s (A-Side)", title),
                           300, 0, 0); //TODO: modify ranges

  auto hCside1D = new TH1F(Form("h_Cside_1D_%s", name.c_str()), Form("%s (C-Side)", title),
                           300, 0, 0); //TODO: modify ranges

  auto hAside2D = new TH2F(Form("h_Aside_2D_%s;x (cm);y (cm)", name.c_str()), Form("%s (A-Side)", title),
                           300, -300, 300, 300, -300, 300);

  auto hCside2D = new TH2F(Form("h_Cside_2D_%s;x (cm);y (cm)", name.c_str()), Form("%s (C-Side)", title),
                           300, -300, 300, 300, -300, 300);

  for (ROC roc; !roc.looped(); ++roc) {

    auto hist2D = hAside2D;
    auto hist1D = hAside1D;
    if (roc.side() == Side::C) {
      hist2D = hCside2D;
      hist1D = hCside1D;
    }

    const int nrows = mapper.getNumberOfRowsROC(roc);
    for (int irow = 0; irow < nrows; ++irow) {
      const int npads = mapper.getNumberOfPadsInRowROC(roc, irow);
      for (int ipad = 0; ipad < npads; ++ipad) {
        const auto val = calDet.getValue(roc, irow, ipad);
        const GlobalPosition2D pos = mapper.getPadCentre(PadROCPos(roc, irow, ipad));
        const int bin = hist2D->FindBin(pos.X(), pos.Y());
        if (!hist2D->GetBinContent(bin))
          hist2D->SetBinContent(bin, val);
        hist1D->Fill(val);
      }
    }
  }

  // ===| Draw histograms |=====================================================
  auto c = new TCanvas(Form("c_%s", name.c_str()), title);
  c->Divide(2, 2);

  c->cd(1);
  hAside2D->Draw("colz");

  c->cd(2);
  hCside2D->Draw("colz");

  c->cd(3);
  hAside1D->Draw();

  c->cd(4);
  hCside1D->Draw();

  // reset the buffer size
  TH1::SetDefaultBufferSize(bufferSize);

  return c;
}

//______________________________________________________________________________
void Clusters::dumpToFile(std::string filename)
{
  if (filename.find(".root") != std::string::npos) {
    filename.resize(filename.size() - 5);
  }

  std::string canvasFile = filename + "_canvas.root";
  auto f = std::unique_ptr<TFile>(TFile::Open(canvasFile.c_str(), "recreate"));
  f->WriteObject(Clusters::paint(mNClusters), "N_Clusters");
  f->WriteObject(Clusters::paint(mQMax), "Q_Max");
  f->WriteObject(Clusters::paint(mQTot), "Q_Tot");
  f->WriteObject(Clusters::paint(mSigmaTime), "Sigma_Time");
  f->WriteObject(Clusters::paint(mSigmaPad), "Sigma_Pad");
  f->WriteObject(Clusters::paint(mTimeBin), "TimeBin");
  f->Close();

  std::string calPadFile = filename + ".root";
  auto g = std::unique_ptr<TFile>(TFile::Open(calPadFile.c_str(), "recreate"));
  g->WriteObject(&mNClusters.getData(), "N_Clusters");
  g->WriteObject(&mQMax.getData(), "Q_Max");
  g->WriteObject(&mQTot.getData(), "Q_Tot");
  g->WriteObject(&mSigmaTime.getData(), "Sigma_Time");
  g->WriteObject(&mSigmaPad.getData(), "Sigma_Pad");
  g->WriteObject(&mTimeBin.getData(), "TimeBin");
  g->Close();
}