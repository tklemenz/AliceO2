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
  }*/
  std::sort(clqVec.begin(), clqVec.end());
  /*for (int i=0; i<clqVec.size(); i++) {
    std::cout<<"sorted charge "<<i<<": "<<clqVec.at(i)<<std::endl;
  }*/
  float mean = 0;
  for (int i = trunclow; i < trunchigh; i++) {
    mean += clqVec.at(i);
  }
  return (mean / (trunchigh - trunclow));
}

void PID(const char *Filename="PID_output.root", const char *OutputPath="/home/tklemenz/ResultsDumpster") {

  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(0.5);
  gStyle->SetTitleSize(24);

  auto& cdb = CDBInterface::instance();
  cdb.setUseDefaults();
  auto& sampa = SAMPAProcessing::instance();
  const auto& mapper = Mapper::instance();

  // ===| initiate histograms |=================================================

  auto tuple = new TNtuple("tuple", "tuple", "qMax:qTot:pad:row");

  auto *hNClusters = new TH1F("hNClusters","; NCluster per track; counts",250,0,250);

  auto *hChargeTot = new TH1F("hChargeTot","; charge tot; counts",3000,0,3000);
  auto *hChargeMax = new TH1F("hChargeMax","; charge max; counts",3000,0,3000);

  auto *hOcc = new TH2F("hOcc","; pad row; pad", 160,0,160,200,-100,100);

  auto *hdEdxTot = new TH1F("hdEdxTot","; dEdxTot; counts",3000,0,3000);
  auto *hdEdxMax = new TH1F("hdEdxMax","; dEdxMax; counts",3000,0,3000);

  auto *hTrackIDs = new TH1F("hTrackIDs","; track ID; counts", 50,0,50);

  auto *grClusA = new TGraph();
  auto *grClusC = new TGraph();

  auto *grClusAnoTrack = new TGraph();
  auto *grClusCnoTrack = new TGraph();

  auto *grTrackAsID = new TGraph();
  auto *grTrackCsID = new TGraph();
  auto *grClusAsID  = new TGraph();
  auto *grClusCsID  = new TGraph();

  // ===| initiate counters and other variables |===============================

  int counter = 0;
  int counterClusterA = 0;
  int counterClusterC = 0;
  int counterClusterAnoTrack = 0;
  int counterClusterCnoTrack = 0;
  int counterTrackAsID = 0;
  int counterTrackCsID = 0;
  int counterClusAsID = 0;
  int counterClusCsID = 0;
  int counterCluster = 0;
  int counterTrack = 0;
  int NClusters = 0;

  int superTrackID = 27;
  int trackWithSuperTrackID = 0;

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

      int trackID = -99;
      const auto& trackLabel = mcTPC->getLabels(k);
      for(auto& label : trackLabel) {
        trackID = label.getTrackID();
        std::cout<<"trackID: "<<trackID<<std::endl;
      }
      hTrackIDs->Fill(trackID);
      if (trackID == superTrackID) { ++trackWithSuperTrackID; }

      //===========================| calculate truncated mean |===============================================
      for (int j = track.getNClusterReferences() - 1; j >= 0; j--) {
        uint8_t sector, row;
        uint32_t clusterIndexInRow;
        int clusterTrackID = -99;
        track.getClusterReference(j, sector, row, clusterIndexInRow);
        auto cl = track.getCluster(j, clusterIndex, sector, row);
        auto clusterLabel = clusterMCBuffer.getLabels(clusterIndex.clusterOffset[sector][row] + clusterIndexInRow);
        for(auto& label : clusterLabel) {
          clusterTrackID = label.getTrackID();
        }
        if (trackID==clusterTrackID) {
          clusterqTotVec.emplace_back(cl.qTot);
          clusterqMaxVec.emplace_back(cl.qMax);
        }
      }
      //std::cout<<"number of clusters in qTot vector: "<<clusterqTotVec.size()<<std::endl;
      //std::cout<<"number of clusters in qMax vector: "<<clusterqMaxVec.size()<<std::endl;
      float truncMeanTot = GetTruncatedMean(clusterqTotVec,trunclow,trunchigh);
      float truncMeanMax = GetTruncatedMean(clusterqMaxVec,trunclow,trunchigh);
      hdEdxTot->Fill(truncMeanTot);
      hdEdxMax->Fill(truncMeanMax);
      //======================================================================================================
      
      
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

        auto clusterLabel = clusterMCBuffer.getLabels(clusterIndex.clusterOffset[sector][row] + clusterIndexInRow);
        

        for(auto& label : clusterLabel) {
          clusterTrackID = label.getTrackID();
          std::cout<<"clusterTrackID: "<<clusterTrackID<<std::endl;
        }


        tuple->Fill(cl.qMax,cl.qTot,padLoc,rowCl);
        hChargeTot->Fill(cl.qTot);
        hChargeMax->Fill(cl.qMax);

        if (clusterTrackID == trackID) {
          hOcc->Fill(row,padLoc);
          if (sec.side()==Side::A) {
            grClusA->SetPoint(counterClusterA,posGlob.X(),posGlob.Y());
            ++counterClusterA;
          }
          else if (sec.side()==Side::C) {
            grClusC->SetPoint(counterClusterC,posGlob.X(),posGlob.Y());
            ++counterClusterC;
          }
          if (trackID == superTrackID) {
            if (sec.side()==Side::A) {
              grTrackAsID->SetPoint(counterTrackAsID,posGlob.X(),posGlob.Y());
              ++counterTrackAsID;
            }
            else if (sec.side()==Side::C) {
              grTrackCsID->SetPoint(counterTrackCsID,posGlob.X(),posGlob.Y());
              ++counterTrackCsID;
            }
          }
        }
        else {
          if (sec.side()==Side::A) {
            grClusAnoTrack->SetPoint(counterClusterAnoTrack,posGlob.X(),posGlob.Y());
            ++counterClusterAnoTrack;
          }
          else if (sec.side()==Side::C) {
            grClusCnoTrack->SetPoint(counterClusterCnoTrack,posGlob.X(),posGlob.Y());
            ++counterClusterCnoTrack;
          }
          if (trackID == superTrackID) {
            if (sec.side()==Side::A) {
              grClusAsID->SetPoint(counterClusAsID,posGlob.X(),posGlob.Y());
              ++counterClusAsID;
            }
            else if (sec.side()==Side::C) {
              grClusCsID->SetPoint(counterClusCsID,posGlob.X(),posGlob.Y());
              ++counterClusCsID;
            }
          }
        }

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
  grClusAnoTrack->SetMarkerColor(kGreen);
  grClusAnoTrack->Draw("Psame");

  c->cd(2);
  grClusC->SetTitle("Clusters C-side");
  grClusC->SetMarkerColor(kRed+2);
  grClusC->GetYaxis()->SetLimits(-300,300);
  grClusC->GetXaxis()->SetLimits(-300,300);
  grClusC->GetYaxis()->SetRangeUser(-300,300);
  grClusC->GetXaxis()->SetRangeUser(-300,300);
  grClusC->Draw("AP");
  grClusCnoTrack->SetMarkerColor(kRed);
  grClusCnoTrack->Draw("Psame");
  //c->BuildLegend();

  auto c2 = new TCanvas("c2",Form("clusters track ID %i",superTrackID), 1200,600);
  c2->Divide(2,1);
  c2->cd(1);
  grTrackAsID->SetTitle(Form("Cl assigned to Track %i - A side",superTrackID));
  grTrackAsID->SetLineColor(kWhite);
  grTrackAsID->SetMarkerColor(kGreen+2);
  grTrackAsID->GetYaxis()->SetLimits(-300,300);
  grTrackAsID->GetXaxis()->SetLimits(-300,300);
  grTrackAsID->GetYaxis()->SetRangeUser(-300,300);
  grTrackAsID->GetXaxis()->SetRangeUser(-300,300);
  grTrackAsID->Draw("AP");
  grClusAsID->SetMarkerColor(kGreen+2);
  grClusAsID->Draw("Psame");
  c2->cd(2);
  grTrackCsID->SetTitle(Form("Cl assigned to Track %i - C side",superTrackID));
  grTrackCsID->SetLineColor(kWhite);
  grTrackCsID->SetMarkerColor(kRed+2);
  grTrackCsID->GetYaxis()->SetLimits(-300,300);
  grTrackCsID->GetXaxis()->SetLimits(-300,300);
  grTrackCsID->GetYaxis()->SetRangeUser(-300,300);
  grTrackCsID->GetXaxis()->SetRangeUser(-300,300);
  grTrackCsID->Draw("AP");
  grClusCsID->SetMarkerColor(kRed+2);
  grClusCsID->Draw("Psame");
  c2->BuildLegend();

  auto *OutFile = new TFile(Form("%s/%s", OutputPath,Filename), "RECREATE");

  OutFile->WriteObject(hChargeTot, "chargeTot");
  OutFile->WriteObject(hChargeMax, "chargeMax");
  OutFile->WriteObject(hNClusters, "NClusters");
  OutFile->WriteObject(hOcc, "hOcc");
  OutFile->WriteObject(hdEdxTot, "hdEdxTot");
  OutFile->WriteObject(hdEdxMax, "hdEdxMax");
  OutFile->WriteObject(hTrackIDs, "hTrackIDs");
  tuple->Write();
  OutFile->WriteObject(c, "all_tracks");
  OutFile->WriteObject(c2, Form("trackID_%i",superTrackID));
  OutFile->Close();

  std::cout<<trackWithSuperTrackID<<" tracks with track ID "<<superTrackID<<"."<<std::endl;
}
