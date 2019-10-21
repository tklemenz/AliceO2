using namespace o2::tpc;

float GetTruncatedMean(std::vector<float> clqVec, float trunclow, float trunchigh) {
  trunclow = clqVec.size() * trunclow/128;
  trunchigh = clqVec.size() * trunchigh/128;
  if (trunclow >= trunchigh) {
    //std::cout<<"trunclow >= trunchigh: "<<trunclow<<" <= "<<trunchigh<<std::endl;
    return (0.);
  }
  /*for (int i=0; i<clqVec.size(); i++) {
    std::cout<<"unsorted charge "<<i<<": "<<clqVec.at(i)<<std::endl;
  }
  std::sort(clqVec.begin(), clqVec.end());
  for (int i=0; i<clqVec.size(); i++) {
    std::cout<<"sorted charge "<<i<<": "<<clqVec.at(i)<<std::endl;
  }*/
  float mean = 0;
  for (int i = trunclow; i < trunchigh; i++) {
    mean += clqVec.at(i);
  }
  return (mean / (trunchigh - trunclow));
}

void PID(const char *Filename="PID_output_1000Events.root", const char *OutputPath="/home/tklemenz/ResultsDumpster") {

  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(0.5);
  gStyle->SetTitleSize(24);

  auto& cdb = CDBInterface::instance();
  cdb.setUseDefaults();
  auto& sampa = SAMPAProcessing::instance();
  const auto& mapper = Mapper::instance();

  // ===| initiate histograms |=================================================

  auto *hNClusters = new TH1F("hNClusters","; NCluster per track; counts",250,0,250);

  auto *hChargeTot = new TH1F("hChargeTot","; charge tot; counts",600,0,600);
  auto *hChargeMax = new TH1F("hChargeMax","; charge max; counts",300,0,300);

  auto *hOcc = new TH2F("hOcc","; pad row; pad", 160,0,160,200,-100,100);

  auto *hdEdxTot = new TH1F("hdEdxTot","; dEdxTot; counts",500,0,500);
  auto *hdEdxMax = new TH1F("hdEdxMax","; dEdxMax; counts",500,0,500);

  auto *grClusA = new TGraph();
  auto *grClusC = new TGraph();

  // ===| initiate counters and other variables |===============================

  int counter = 0;
  int counterClusterA = 0;
  int counterClusterC = 0;
  int counterCluster = 0;
  int counterTrack = 0;
  int NClusters = 0;

  //float ChargeTot[o2::tpc::Constants::MAXGLOBALPADROW];
  std::vector<float> clusterqTotVec;
  std::vector<float> clusterqMaxVec;

  float trunclow = 2.;
  float trunchigh = 77.;

  // ===| process the tracks |==================================================
  auto file   = TFile::Open("tpctracks.root");
  auto tree = (TTree *)file->Get("tpcrec");
  std::vector<o2::tpc::TrackTPC> *tpcTracks = nullptr;
  tree->SetBranchAddress("TPCTracks", &tpcTracks);

  o2::dataformats::MCTruthContainer<o2::MCCompLabel>* mcTPC = nullptr;
  tree->SetBranchAddress("TPCTracksMCTruth", &mcTPC);

  o2::tpc::ClusterNativeHelper::Reader tpcClusterReader;
  tpcClusterReader.init("tpc-native-clusters.root");
 
  o2::tpc::ClusterNativeAccess clusterIndex;
  std::unique_ptr<o2::tpc::ClusterNative[]> clusterBuffer;
  o2::tpc::MCLabelContainer clusterMCBuffer;
  memset(&clusterIndex, 0, sizeof(clusterIndex));

  for (int i=0; i<tree->GetEntriesFast(); ++i) {
    std::cout << "Event " << i << "\n";
    tree->GetEntry(i);
    tpcClusterReader.read(i);
    tpcClusterReader.fillIndex(clusterIndex, clusterBuffer, clusterMCBuffer);
    for (int k = 0; k < tpcTracks->size(); k++) {
      auto track =(*tpcTracks)[k];
      clusterqTotVec.clear();
      clusterqMaxVec.clear();
      NClusters = track.getNClusterReferences();
      hNClusters->Fill(NClusters);

      //std::cout << "   Track " << k << "\n";
      //std::cout << "      Number of clusters: " << track.getNClusterReferences() << "\n";


      //===========================| calculate truncated mean |===============================================
      for (int j = track.getNClusterReferences() - 1; j >= 0; j--) {
        uint8_t str, r;
        uint32_t ClusterIndexInRow;
        track.getClusterReference(j, str, r, ClusterIndexInRow);
        auto cl = track.getCluster(j, clusterIndex, str, r);
        clusterqTotVec.emplace_back(cl.qTot);
        clusterqMaxVec.emplace_back(cl.qMax);
      }
      std::cout<<"number of clusters in qTot vector: "<<clusterqTotVec.size()<<std::endl;
      std::cout<<"number of clusters in qMax vector: "<<clusterqMaxVec.size()<<std::endl;
      float truncMeanTot = GetTruncatedMean(clusterqTotVec,trunclow,trunchigh);
      float truncMeanMax = GetTruncatedMean(clusterqMaxVec,trunclow,trunchigh);
      hdEdxTot->Fill(truncMeanTot);
      hdEdxMax->Fill(truncMeanMax);
      //======================================================================================================
      
      int trackID = -99;
      const auto& trackLabel = mcTPC->getLabels(k);
      /*for(auto& label : trackLabel) {
        trackID = label.getTrackID();
        std::cout<<"trackID: "<<trackID<<std::endl;
      }*/
      NClusters = 0;
      for (int j = track.getNClusterReferences() - 1; j >= 0; j--) {
        uint8_t sector, row;
        uint32_t clusterIndexInRow;
        int clusterTrackID = -99;
        track.getClusterReference(j, sector, row, clusterIndexInRow);
        auto cl = track.getCluster(j, clusterIndex, sector, row);
        //std::cout << "         " << j << ": ("  << int(row) << ", " << cl.getPad() << ")   [";
        const Sector sec = Sector(int(sector));
        const int rowCl             = int(row);
        float padLoc				= cl.getPad();
        const GlobalPadNumber pad   = mapper.globalPadNumber(PadPos(rowCl, cl.getPad()));
        const PadCentre& padCentre  = mapper.padCentre(pad);
        const float localYfactor    = (sec.side()==Side::A)?-1.f:1.f;
        float zPosition       = sampa.getZfromTimeBin(cl.getTime(), sec.side());
        LocalPosition3D posLoc(padCentre.X(), localYfactor*padCentre.Y(), zPosition);
        GlobalPosition3D posGlob = Mapper::LocalToGlobal(posLoc, sec);

        /*auto clusterLabel = clusterMCBuffer.getLabels(clusterIndex.clusterOffset[sector][row] + clusterIndexInRow);
        

        for(auto& label : clusterLabel) {
          clusterTrackID = label.getTrackID();
          std::cout<<"clusterTrackID: "<<clusterTrackID<<std::endl;
        }*/



        //if (clusterTrackID == trackID) {

          hChargeTot->Fill(cl.qTot);
          hChargeMax->Fill(cl.qMax);
          hOcc->Fill(row,padLoc);
          if (sec.side()==Side::A) {
            grClusA->SetPoint(counterClusterA,posGlob.X(),posGlob.Y());
            ++counterClusterA;
          }
          else if (sec.side()==Side::C) {
            grClusC->SetPoint(counterClusterC,posGlob.X(),posGlob.Y());
            ++counterClusterC;
          }
        //}

        ++counterCluster;
      }
    }
  }
  auto c = new TCanvas("c","clusters",1200,600);
  c->Divide(2,1);
  c->cd(1);
  grClusA->SetTitle("Clusters A-side");
  grClusA->SetMarkerColor(kGreen+2);
  grClusA->GetYaxis()->SetLimits(-300,300);
  grClusA->GetXaxis()->SetLimits(-300,300);
  grClusA->GetYaxis()->SetRangeUser(-300,300);
  grClusA->GetXaxis()->SetRangeUser(-300,300);
  grClusA->Draw("AP");
  c->cd(2);
  grClusC->SetTitle("Clusters C-side");
  grClusC->SetMarkerColor(kRed+2);
  grClusC->GetYaxis()->SetLimits(-300,300);
  grClusC->GetXaxis()->SetLimits(-300,300);
  grClusC->GetYaxis()->SetRangeUser(-300,300);
  grClusC->GetXaxis()->SetRangeUser(-300,300);
  grClusC->Draw("AP");
  //c->BuildLegend();

  auto *OutFile = new TFile(Form("%s/%s", OutputPath,Filename), "RECREATE");

  OutFile->WriteObject(hChargeTot, "chargeTot");
  OutFile->WriteObject(hChargeMax, "chargeMax");
  OutFile->WriteObject(hNClusters, "NClusters");
  OutFile->WriteObject(hOcc, "hOcc");
  OutFile->WriteObject(hdEdxTot, "hdEdxTot");
  OutFile->WriteObject(hdEdxMax, "hdEdxMax");
  OutFile->Close();
}
