using namespace o2::tpc;

void runPID(std::string_view outputFileName = "PID", std::string_view inputFileName = "tpctracks.root", const size_t maxTracks = 0)
{
  // ===| track file and tree |=================================================
  auto file = TFile::Open(inputFileName.data());
  auto tree = (TTree*)file->Get("tpcrec");
  if (tree == nullptr) {
    std::cout << "Error getting tree\n";
    return;
  }

  // ===| branch setup |==========================================================
  std::vector<TrackTPC>* tpcTracks = nullptr;
  tree->SetBranchAddress("TPCTracks", &tpcTracks);

  // ===| create PID object |=====================================================
  o2::tpc::qc::PID pid;

  pid.initializeHistograms();
  pid.setNiceStyle();

  // ===| event loop |============================================================
  for (int i = 0; i < tree->GetEntriesFast(); ++i) {
    tree->GetEntry(i);
    size_t nTracks = (maxTracks > 0) ? std::min(tpcTracks->size(), maxTracks) : tpcTracks->size();
    // ---| track loop |---
    for (int k = 0; k < nTracks; k++) {
      auto track = (*tpcTracks)[k];
      pid.processTrack(track);
    }
  }

  // ===| create canvas |========================================================
  auto* c1 = new TCanvas("c1", "PID", 1200, 600);
  c1->Divide(2, 2);
  auto histos2D = pid.getHistograms2D();
  c1->cd(1);
  histos2D[0].Draw();
  gPad->SetLogz();
  c1->cd(2);
  histos2D[1].Draw();
  gPad->SetLogz();
  c1->cd(3);
  histos2D[2].Draw();
  gPad->SetLogz();
  c1->cd(4);
  histos2D[3].Draw();
  gPad->SetLogz();
  gPad->SetLogx();

  auto f = std::unique_ptr<TFile>(TFile::Open(Form("%s_canvas.root", outputFileName.data()), "recreate"));
  f->WriteObject(c1, "PID");
  f->Close();
  delete c1;

  //===| dump histograms to a file |=============================================
  pid.dumpToFile(Form("%s.root", outputFileName.data()));

  pid.resetHistograms();
}
