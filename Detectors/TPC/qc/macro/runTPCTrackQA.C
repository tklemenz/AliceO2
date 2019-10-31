using namespace o2::tpc;

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
  TPCTrackQA tpcTrackQA;

  tpcTrackQA.initializeHistograms();

  // ===| event loop |============================================================
  for (int i=0; i<tree->GetEntriesFast(); ++i) {
    std::cout << "Event " << i << "\n";
    tree->GetEntry(i);
    size_t nTracks = (maxTracks > 0) ? std::min(tpcTracks->size(), maxTracks) : tpcTracks->size();
    // ---| track loop |---
    for (int k = 0;k < nTracks;k++) {
      auto track =(*tpcTracks)[k];
      tpcTrackQA.processTrack(track);
    }
  }

  //===| dump histograms to a file |=============================================
  tpcTrackQA.dumpToFile(outputfileName.data());

  //===| reset histograms |======================================================
  tpcTrackQA.resetHistograms();
}