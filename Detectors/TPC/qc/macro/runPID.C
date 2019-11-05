using namespace o2::tpc;

void runPID(std::string_view outputfileName = "PID.root", std::string_view inputfileName = "tpctracks.root", std::string_view draw = "draw", const size_t maxTracks = 0)
{
  // ===| track file and tree |=================================================
  auto file = TFile::Open(inputfileName.data());
  auto tree = (TTree*)file->Get("tpcrec");
  if (tree == nullptr) {
    std::cout << "Error getting tree\n";
    return;
  }

  // ===| branch setup |==========================================================
  std::vector<TrackTPC>* tpcTracks = nullptr;
  tree->SetBranchAddress("TPCTracks", &tpcTracks);

  // ===| create PID object |==============================================
  auto& pid = o2::tpc::qc::PID::instance();

  pid.initializeHistograms();

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

  //===| dump histograms to a file |=============================================
  pid.dumpToFile(outputfileName.data(), draw.data());

}
