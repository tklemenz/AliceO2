void runClusters(std::string_view fileName = "tpc-native-clusters.root", std::string_view outputFile = "ClusterQC", const size_t maxNClusters = 0)
{
  o2::tpc::ClusterNativeHelper::Reader tpcClusterReader;
  tpcClusterReader.init(fileName.data());

  o2::tpc::ClusterNativeAccess clusterIndex;
  std::unique_ptr<o2::tpc::ClusterNative[]> clusterBuffer;
  memset(&clusterIndex, 0, sizeof(clusterIndex));
  o2::tpc::MCLabelContainer clusterMCBuffer;

  o2::tpc::qc::Clusters clusters;

  for (int i = 0; i < tpcClusterReader.getTreeSize(); ++i) {
    std::cout << "Event " << i << "\n";
    tpcClusterReader.read(i);
    tpcClusterReader.fillIndex(clusterIndex, clusterBuffer, clusterMCBuffer);
    size_t iClusters = 0;
    for (int isector = 0; isector < o2::tpc::Constants::MAXSECTOR; ++isector) {
      for (int irow = 0; irow < o2::tpc::Constants::MAXGLOBALPADROW; ++irow) {
        const int nClusters = clusterIndex.nClusters[isector][irow];
        if (!nClusters) {
          continue;
        }
        for (int icl = 0; icl < nClusters; ++icl) {
          const auto& cl = *(clusterIndex.clusters[isector][irow] + icl);
          clusters.processCluster(cl, o2::tpc::Sector(isector), irow);
          ++iClusters;
          if (maxNClusters > 0 && iClusters >= maxNClusters) {
            return;
          }
        }
      }
    }
  }
  clusters.analyse();
  clusters.dumpToFile(outputFile.data());
  //gROOT->cd();
  //clusters.draw();
  /*o2::tpc::painter::draw(clusters.getNLocalMaxima());
  o2::tpc::painter::draw(clusters.getMaxCharge());
  o2::tpc::painter::draw(clusters.getMeanCharge());
  o2::tpc::painter::draw(clusters.getNTimeBins());
  o2::tpc::painter::draw(clusters.getNPads());
  o2::tpc::painter::draw(clusters.getTimePosition());
*/
}
