using namespace o2::tpc;

/*void BinLogX(TH2 *h){
  TAxis *axis = h->GetXaxis();
  int bins = axis->GetNbins();

  Axis_t from = axis->GetXmin();
  Axis_t to = axis->GetXmax();
  Axis_t width = (to - from) / bins;
  Axis_t *new_bins = new Axis_t[bins +1];

  for (int i = 0; i<= bins; i++) {
  	new_bins[i] = TMath::Power(10, from + i * width);
  }
  axis->Set(bins,new_bins);
  delete new_bins;
}*/

void runTPCTrackQA(std::string_view outputfileName = "TPCTrackQA.root", std::string_view inputfileName = "tpctracks.root", const size_t maxTracks = 0) {
	// ===| track file and tree |=================================================
  auto file   = TFile::Open(inputfileName.data());
  auto tree = (TTree *)file->Get("tpcrec");
  if (tree == nullptr) {
    std::cout << "Error getting tree\n";
    return;
  }

  // ===| branch setup |==========================================================
  std::vector<TrackTPC> *tpcTracks = nullptr;
  tree->SetBranchAddress("TPCTracks", &tpcTracks);

  // ===| create TPCTrackQA object |==============================================
  auto& tpcTrackQA = o2::tpc::qc::TPCTrackQA::instance();

  tpcTrackQA.initializeHistograms();

  // ===| event loop |============================================================
  for (int i=0; i<tree->GetEntriesFast(); ++i) {
    tree->GetEntry(i);
    size_t nTracks = (maxTracks > 0) ? std::min(tpcTracks->size(), maxTracks) : tpcTracks->size();
    // ---| track loop |---
    for (int k = 0;k < nTracks;k++) {
      auto track =(*tpcTracks)[k];
      tpcTrackQA.processTrack(track);
    }
  }

  //===| beautify histograms |===================================================
  /*std::vector<TH2F>& histos2D = tpcTrackQA.getHistograms2D();
  BinLogX(&histos2D[3]);
  TCanvas* c1 = new TCanvas("c1","PID",1200,600);
  c1->Divide(2,2);
  c1->cd(1);
  histos2D[0].Draw("colz");
  histos2D[0].SetTitle("dedx (a.u.) vs #phi (rad)");
  gPad->SetLogz();
  c1->cd(2);
  histos2D[1].Draw("colz");
  histos2D[1].SetTitle("dedx (a.u.) vs tan#lambda");
  gPad->SetLogz();
  c1->cd(3);
  histos2D[2].Draw("colz");
  histos2D[2].SetTitle("dedx (a.u.) vs ncls");
  gPad->SetLogz();
  c1->cd(4);
  histos2D[3].Draw("colz");
  histos2D[3].SetTitle("dedx (a.u.) vs p (G#it{e}V/#it{c})");
  gPad->SetLogz();
  gPad->SetLogx();
*/

  //===| draw histograms |=======================================================
  tpcTrackQA.setNiceStyle();
  tpcTrackQA.drawHistograms();


  //===| dump histograms to a file |=============================================
  tpcTrackQA.dumpToFile(outputfileName.data());

  //===| reset histograms |======================================================
  //tpcTrackQA.resetHistograms();
}
