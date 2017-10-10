// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See https://alice-o2.web.cern.ch/ for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file TrackTPC.cxx
/// \brief Implementation of the TPC track
/// \author Thomas Klemenz, TU Muenchen, thomas.klemenz@tum.de

#include "TPCReconstruction/TrackTPC.h"
#include "TPCBase/ROC.h"
#include "TPCBase/Mapper.h"

#include <TROOT.h>
#include <TString.h>
#include <boost/lambda/lambda.hpp>

using namespace o2::TPC;

CalDet<float> *TrackTPC::mGainMap = new CalDet<float>(PadSubset::ROC);

float TrackTPC::getTruncatedMean(int runNr, float low, float high, int type, bool removeRows, bool removeEdge, bool removeEnd, TH2D *excludeHisto, float ChargeCorr, int edgeCut, int *nclPID, TH1F *TruncDist, TH1F *ChargeDist) const
{
  ROC roc(0);
  float norm = 1.;

  std::vector<float> values;
  Mapper &mapper = Mapper::instance();

  for (auto &clusterObject : mClusterVector) {
    DigitPos pos(clusterObject.getCRU(), PadPos(clusterObject.getRow(), clusterObject.getPadMean()));
    float row = pos.getPadSecPos().getPadPos().getRow();
    float pad = pos.getPadSecPos().getPadPos().getPad();
    float cpad = pad - mapper.getNumberOfPadsInRowSector(row)/2;
    if (removeRows) {
      if (row == 31 || row == 32) {
        excludeHisto->Fill(row, cpad, 1);
        continue;
      }


//        int nexthit = HitMapS0->getValue(roc, row, pad+1);
//        int previoushit = HitMapS0->getValue(roc, row, pad-1);
//        if (nexthit != 1 || previoushit != 1) {
//          excludeHisto->Fill(row, cpad, 1);
//          continue;
//        }

      if (runNr > 255) {
        if (row > 1 && row < 61) {
          if (cpad == 0 || cpad == -1 || cpad == -2) {
            excludeHisto->Fill(row, cpad, 1);
            continue;
          }
        }
//        int nexthit = HitMapS1->getValue(roc, row, pad+1);
//        int previoushit = HitMapS1->getValue(roc, row, pad-1);
//        if (nexthit != 1 || previoushit != 1) {
//          excludeHisto->Fill(row, cpad, 1);
//          continue;
//        }
      }
    }
    if (removeEdge) {
      if (runNr <= 243) {

        if (row < 5) {
          if (edgeCut == 1) {
            if (!(cpad > 7 && cpad < 19)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 8 && cpad < 18)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 9 && cpad < 17)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 5) {
          if (edgeCut == 1) {
            if (!(cpad > 7 && cpad < 19)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 8 && cpad < 18)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 9 && cpad < 17)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row > 5 && row < 10) {
          if (edgeCut == 1) {
            if (!(cpad > 7 && cpad < 20)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 8 && cpad < 19)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 9 && cpad < 18)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 10) {
          if (edgeCut == 1) {
            if (!(cpad > 7 && cpad < 20)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 8 && cpad < 19)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 9 && cpad < 18)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row > 10 && row < 16) {
          if (edgeCut == 1) {
            if (!(cpad > 7 && cpad < 21)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 8 && cpad < 20)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 9 && cpad < 19)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 15) {
          if (edgeCut == 1) {
            if (!(cpad > 8 && cpad < 21)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 9 && cpad < 20)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 10 && cpad < 19)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 16) {
          if (edgeCut == 1) {
            if (cpad > 7 && cpad < 20) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (cpad > 8 && cpad < 19) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (cpad > 9 && cpad < 18) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row > 15 && row <= 22) {
          if (edgeCut == 1) {
            if (!(cpad > 8 && cpad < 22)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 9 && cpad < 21)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 10 && cpad < 20)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row > 22 && row <= 27) {
          if (edgeCut == 1) {
            if (!(cpad > 8 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 9 && cpad < 22)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 10 && cpad < 21)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row > 27 && row <= 30) {
          if (edgeCut == 1) {
            if (!(cpad > 8 && cpad < 24)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 9 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 10 && cpad < 22)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 31) {
          if (edgeCut == 1) {
            if (!(cpad > 9 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 10 && cpad < 22)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 11 && cpad < 21)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 32) {
          if (edgeCut == 1) {
            if (!(cpad > 10 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 11 && cpad < 22)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 12 && cpad < 21)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row > 32 && row <= 37) {
          if (edgeCut == 1) {
            if (!(cpad > 10 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 11 && cpad < 22)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 12 && cpad < 21)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row > 37 && row <= 43) {
          if (edgeCut == 1) {
            if (!(cpad > 10 && cpad < 24)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 11 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 12 && cpad < 22)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 44 || row == 45) {
          if (edgeCut == 1) {
            if (!(cpad > 10 && cpad < 25)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 11 && cpad < 24)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 12 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 46) {
          if (edgeCut == 1) {
            if (!(cpad > 11 && cpad < 25)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 12 && cpad < 24)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 13 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 47 || row == 48) {
          if (edgeCut == 1) {
            if (!(cpad > 11 && cpad < 24)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 12 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 13 && cpad < 22)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
            return true;
          }
        }

        else if (row == 49) {
          if (edgeCut == 1) {
            if (!(cpad > 10 && cpad < 24)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 11 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 12 && cpad < 22)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 50 || row == 51) {
          if (edgeCut == 1) {
            if (!(cpad > 10 && cpad < 25)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 11 && cpad < 24)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 12 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row >= 52 && row <= 55) {
          if (edgeCut == 1) {
            if (!(cpad > 11 && cpad < 25)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 12 && cpad < 24)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 13 && cpad < 23)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row > 55 && row <= 60) {
          if (edgeCut == 1) {
            if (!(cpad > 11 && cpad < 26)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 12 && cpad < 25)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 13 && cpad < 24)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 61 || row == 62) {
          if (edgeCut == 1) {
            if (!(cpad > 11 && cpad < 27)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > 12 && cpad < 26)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 13 && cpad < 25)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }
      }
      else if (runNr >= 255) {
        if (row < 6) {
          if (edgeCut == 1) {
            if (!(cpad > -2 && cpad < 10)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -1 && cpad < 9)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 0 && cpad < 8)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row >= 6 && row <=10) {
          if (edgeCut == 1) {
            if (!(cpad > -2 && cpad < 11)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -1 && cpad < 10)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 0 && cpad < 9)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 11) {
          if (edgeCut == 1) {
            if (!(cpad > -2 && cpad < 10)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -1 && cpad < 9)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > 0 && cpad < 8)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row >= 12 && row <= 15) {
          if (edgeCut == 1) {
            if (!(cpad > -3 && cpad < 10)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -2 && cpad < 9)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -1 && cpad < 8)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 16 || row == 17) {
          if (edgeCut == 1) {
            if (!(cpad > -3 && cpad < 7)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -2 && cpad < 6)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -1 && cpad < 5)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row >= 18 && row <= 29) {
          if (edgeCut == 1) {
            if (!(cpad > -8 && cpad < 7)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -7 && cpad < 6)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -6 && cpad < 5)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 30) {
          if (edgeCut == 1) {
            if (!(cpad > -8 && cpad < 6)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -7 && cpad < 5)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -6 && cpad < 4)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 31 || row == 32) {
          if (edgeCut == 1) {
            if (!(cpad > -8 && cpad < 4)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -7 && cpad < 3)) {
              excludeHisto->Fill(row, cpad, 1);continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -6 && cpad < 2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 33) {
          if (edgeCut == 1) {
            if (!(cpad > -9 && cpad < 4)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -8 && cpad < 3)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -7 && cpad < 2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row >= 34 && row <= 46) {
          if (edgeCut == 1) {
            if (!(cpad > -10 && cpad < 4)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -9 && cpad < 3)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -8 && cpad < 2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 47) {
          if (edgeCut == 1) {
            if (!(cpad > -10 && cpad < -1)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -9 && cpad < -2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -8 && cpad < -3)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 48) {
          if (edgeCut == 1) {
            if (!(cpad > -11 && cpad < -1)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -10 && cpad < -2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -9 && cpad < -3)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 49) {
          if (edgeCut == 1) {
            if (!(cpad > -13 && cpad < -1)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -12 && cpad < -2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -11 && cpad < -3)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 50) {
          if (edgeCut == 1) {
            if (!(cpad > -14 && cpad < -1)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -13 && cpad < -2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -12 && cpad < -3)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 51 || row == 52) {
          if (edgeCut == 1) {
            if (!(cpad > -15 && cpad < -1)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -14 && cpad < -2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -13 && cpad < -3)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row >= 53 && row <= 59) {
          if (edgeCut == 1) {
            if (!(cpad > -16 && cpad < -1)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -15 && cpad < -2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -14 && cpad < -3)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 60) {
          if (edgeCut == 1) {
            if (!(cpad > -16 && cpad < -2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -15 && cpad < -3)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -14 && cpad < -4)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }

        else if (row == 61 || row == 62) {
          if (edgeCut == 1) {
            if (!(cpad > -17 && cpad < -2)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 2) {
            if (!(cpad > -16 && cpad < -3)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else if (edgeCut == 3) {
            if (!(cpad > -15 && cpad < -4)) {
              excludeHisto->Fill(row, cpad, 1);
              continue;
            }
          }
          else {
            std::cout<<std::endl<<"function cannot cut more than three pads on each side. choose 1, 2 or 3 \n"<<"no pads were cut!"<<std::endl;
          }
        }
      }
    }
    if (removeEnd) {
      if (runNr <= 243) {
        if (row >= 57 || row < 2) {
          excludeHisto->Fill(row, cpad, 1);
          continue;
        }
      }
      else if (runNr > 255) {
        if (row >= 61 || row < 2) {
          excludeHisto->Fill(row, cpad, 1);
          continue;
        }
      }
    }
    if (mGainMap) {
      norm = mGainMap->getValue(roc, row, pad);
      if (norm < 1e-4) {
        norm = 1.;
      }
      values.push_back((type == 0)?clusterObject.getQmax()/(norm*ChargeCorr):clusterObject.getQ()/(norm*ChargeCorr));
    }
    else {
      values.push_back((type == 0)?clusterObject.getQmax()/ChargeCorr:clusterObject.getQ()/ChargeCorr);
    }
  }

  transform(values.begin(), values.end(), values.begin(), boost::lambda::_1 * cos(atan(TrackTPC::getTgl())) * cos(asin(TrackTPC::getSnp())));

  std::sort(values.begin(), values.end());

  float dEdx = 0.f;
  int nClustersTrunc = 0;
  int nClustersUsed = static_cast<int>(values.size());

  if (ChargeDist != nullptr) {
    for (int icl=0; icl<nClustersUsed; ++icl) {
      ChargeDist->Fill(values[icl]);
    }
  }

  for (int icl=0; icl<nClustersUsed; ++icl) {
    if (icl<std::round(low*nClustersUsed)) continue;
    if (icl>std::round(high*nClustersUsed)) break;

    dEdx+=values[icl];
    ++nClustersTrunc;
    if (TruncDist != nullptr) {
      TruncDist->Fill(values[icl]);
    }
  }

  if (nClustersTrunc>0) {
    dEdx/=nClustersTrunc;
  }

  if (nclPID) (*nclPID)=nClustersTrunc;

  return dEdx;
}

void TrackTPC::setGainMap(TString GainMapFile ,int setting)
{
  TFile f(GainMapFile);
  gROOT->cd();

  ROC roc(0);

  CalDet<float> *gainmapS0 = nullptr, *gainmapS1 = nullptr;
  f.GetObject("GainMapS0", gainmapS0);
  f.GetObject("GainMapS1", gainmapS1);

  CalArray<float>& mGainMapArr = mGainMap->getCalArray(roc);

  if (setting == 0) {
    mGainMapArr = gainmapS0->getCalArray(roc);
  }
  else if (setting == 1) {
    mGainMapArr = gainmapS1->getCalArray(roc);
  }
  else {
    std::cout<<std::endl<<"invalid setting: choose either 0 or 1"<<std::endl;
  }
}
