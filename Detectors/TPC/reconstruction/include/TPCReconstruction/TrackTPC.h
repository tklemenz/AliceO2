// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See https://alice-o2.web.cern.ch/ for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef ALICEO2_TPC_TRACKTPC
#define ALICEO2_TPC_TRACKTPC

#include "DetectorsBase/Track.h"
#include "TPCBase/Defs.h"

#include "TPCReconstruction/Cluster.h"
#include "DataFormatsTPC/ClusterNative.h"
#include "TPCBase/CalDet.h"

#include <TClonesArray.h>
#include <TH2.h>
#include <TH1.h>

namespace o2 {
namespace TPC {

/// \class TrackTPC
/// This is the definition of the TPC Track Object


class TrackTPC {
  public:

     //using o2::Base::Track::TrackParCov::TrackParCov; // inherit

    /// Default constructor
    TrackTPC();

    /// Constructor, initializing values for x, alpha and an array with Y, Z, sin(phi), tg(lambda) and q/pT
    /// \param x X of track evaluation
    /// \param alpha track frame angle
    /// \param std::array par contains Y, Z, sin(phi), tg(lambda) and q/pT
    TrackTPC(float x, float alpha, const std::array<float, o2::Base::Track::kNParams> &par, const std::array<float, o2::Base::Track::kCovMatSize> &cov);

    /// Constructor, initializing values for x,y and z in an array, momenta px, py and pz in an array
    /// \param std::array xyz contains x, y and z coordinates of the vertex of the origin
    /// \param std::array pxpypz contains momenta in x, y and z direction
    /// \param sign sign of the charge of the particle
    /// \param sectorAlpha false: angle of pT direction, true: angle of the sector from X, Y coordinate for r>1; angle of pT direction for r==0
    TrackTPC(const std::array<float,3> &xyz, const std::array<float,3> &pxpypz, const std::array<float, o2::Base::Track::kLabCovMatSize> &cv, int sign, bool sectorAlpha=true);

    /// Destructor
    ~TrackTPC() = default;

    /// Add a single cluster to the track
    void addCluster(const Cluster &c);

    /// Add an array/vector of clusters to the track
    //void addClusterArray(TClonesArray *arr);
    template <typename ClusterType>
    void addClusterArray(std::vector<ClusterType> *arr);

    /// Get the clusters which are associated with the track
    /// \return clusters of the track as a std::vector
    void getClusterVector(std::vector<Cluster> &clVec)  const { clVec = mClusterVector; }

    /// Get the truncated mean energy loss of the track
    /// \param low low end of truncation
    /// \param high high end of truncation
    /// \param type 0 for Qmax, 1 for Q
    /// \param particleType 0 for Pions, 1 for Electrons
    /// \param removeRows option to remove certain rows from the dEdx calculation
    /// \param nclPID pass any pointer to have the number of used clusters written to it
    /// \return mean energy loss
    
    float getTime0() const {return mTime0;}
    float getLastClusterZ() const {return mLastClusterZ;}
    Side getSide() const {return (Side) mSide;}
    void setTime0(float v) {mTime0 = v;}
    void setLastClusterZ(float v) {mLastClusterZ = v;}
    void setSide(Side v) {mSide = v;}
    
    void resetClusterReferences(int nClusters);
    int getNClusterReferences() {return mNClusters;}
    void setClusterReference(int nCluster, uint8_t sectorIndex, uint8_t rowIndex, uint32_t clusterIndex) {
      mClusterReferences[nCluster] = clusterIndex;
      reinterpret_cast<uint8_t*>(mClusterReferences.data())[4 * mNClusters + nCluster] = sectorIndex;
      reinterpret_cast<uint8_t*>(mClusterReferences.data())[5 * mNClusters + nCluster] = rowIndex;
    }
    void getClusterReference(int nCluster, uint8_t& sectorIndex, uint8_t& rowIndex, uint32_t& clusterIndex) const {
      clusterIndex = mClusterReferences[nCluster];
      sectorIndex = reinterpret_cast<const uint8_t*>(mClusterReferences.data())[4 * mNClusters + nCluster];
      rowIndex = reinterpret_cast<const uint8_t*>(mClusterReferences.data())[5 * mNClusters + nCluster];
    }
    const o2::DataFormat::TPC::ClusterNative& getCluster(int nCluster, const o2::DataFormat::TPC::ClusterNativeAccessFullTPC& clusters, uint8_t& sectorIndex, uint8_t& rowIndex) const {
        uint32_t clusterIndex;
        getClusterReference(nCluster, sectorIndex, rowIndex, clusterIndex);
        return(clusters.clusters[sectorIndex][rowIndex][clusterIndex]);
    }
    const o2::DataFormat::TPC::ClusterNative& getCluster(int nCluster, const o2::DataFormat::TPC::ClusterNativeAccessFullTPC& clusters) const {
      uint8_t sectorIndex, rowIndex;
      return(getCluster(nCluster, clusters, sectorIndex, rowIndex));
    }

    float getTruncatedMean(int runNr, float low=0.05, float high=0.7, int type=1, bool removeRows=false, bool removeEdge=false, bool removeEnd=false, TH2D *excludeHisto=nullptr, float ChargeCorr=1, int edgeCut=1, int *nclPID=nullptr, TH1F *TruncDist=nullptr, TH1F *ChargeDist=nullptr) const;

    void setGainMap(TString GainMapFile, int setting);

    /// Get the TrackParCov object
    o2::Base::Track::TrackParCov getTrack() { return mTrackParCov; }



    float getX()                         const { return mTrackParCov.getX(); }
    float getAlpha()                     const { return mTrackParCov.getAlpha(); }
    float getY()                         const { return mTrackParCov.getY(); }
    float getZ()                         const { return mTrackParCov.getZ(); }
    float getSnp()                       const { return mTrackParCov.getSnp(); }
    float getTgl()                       const { return mTrackParCov.getTgl(); }
    float getQ2Pt()                      const { return mTrackParCov.getQ2Pt(); }

    // derived getters
    float getCurvature(float b)          const { return mTrackParCov.getCurvature(float(b));}
    float getSign()                      const { return mTrackParCov.getSign();}
    float getPhi()                       const { return mTrackParCov.getPhi();}
    float getPhiPos()                    const { return mTrackParCov.getPhiPos(); }

    float getP()                         const { return mTrackParCov.getP(); }
    float getPt()                        const { return mTrackParCov.getPt(); }
    void  getXYZ(std::array<float,3> &xyz)           const { mTrackParCov.getXYZGlo(xyz); }
    bool  getPxPyPz(std::array<float,3> &pxyz)       const { return mTrackParCov.getPxPyPzGlo(pxyz); }
    bool  getPosDir(std::array<float,9> &posdirp)    const { return mTrackParCov.getPosDirGlo(posdirp); }

    /// \todo implement getters for covariance (missing access to full covariance in Track.h)


    // parameters manipulation
    bool  rotateParam(float alpha)       { return mTrackParCov.rotate(alpha); }
    bool  propagateParamTo(float xk, float b)        { return mTrackParCov.propagateParamTo(xk, b); }
    bool  propagateParamTo(float xk, const std::array<float,3> &b)    { return mTrackParCov.propagateParamTo(xk, b); }
    void  invertParam()                  { mTrackParCov.invertParam(); }

    void  PrintParam()                   { mTrackParCov.PrintParam(); }


  private:
    o2::Base::Track::TrackParCov mTrackParCov;
    std::vector<Cluster> mClusterVector;
    float mTime0 = 0.f; //Reference Z of the track assumed for the vertex, scaled with pseudo VDrift and reference timeframe length.
    float mLastClusterZ = 0.f; //Z position of last cluster
    char mSide = Side::UNDEFINED;
    
    //New structure to store cluster references
    int mNClusters = 0;
    std::vector<uint32_t> mClusterReferences;

    //static std::unique_ptr<o2::TPC::CalDet<float>> mGainMap;

    static o2::TPC::CalDet<float> *mGainMap;

};

inline
TrackTPC::TrackTPC()
  : mTrackParCov()
  , mClusterVector()
{}

inline
TrackTPC::TrackTPC(float x,float alpha, const std::array<float, o2::Base::Track::kNParams> &par, const std::array<float,o2::Base::Track::kCovMatSize> &cov)
  : mTrackParCov(x, alpha, par, cov)
  , mClusterVector()
{}

inline
TrackTPC::TrackTPC(const std::array<float,3> &xyz,const std::array<float,3> &pxpypz, const std::array<float,o2::Base::Track::kLabCovMatSize> &cv, int sign, bool sectorAlpha)
  : mTrackParCov(xyz, pxpypz, cv, sign, sectorAlpha)
  , mClusterVector()
{}

inline
void TrackTPC::addCluster(const Cluster &c)
{
  mClusterVector.push_back(c);
}

template<typename ClusterType>

inline
//void TrackTPC::addClusterArray(TClonesArray *arr)
void TrackTPC::addClusterArray(std::vector<ClusterType> *arr)
{
  static_assert(std::is_base_of<o2::TPC::Cluster, ClusterType>::value, "ClusterType needs to inherit from o2::TPC::Cluster");
  for (auto clusterObject : *arr){
    addCluster(clusterObject);
    //Cluster *inputcluster = static_cast<Cluster*>(clusterObject);
    //addCluster(*inputcluster);
  }
}

}
}

#endif
