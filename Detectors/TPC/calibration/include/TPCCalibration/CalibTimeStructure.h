#ifndef ALICEO2_TPC_CALIBTIMESTRUCTURE_H_
#define ALICEO2_TPC_CALIBTIMESTRUCTURE_H_

#include "TPCCalibration/CalibPedestal.h"
#include "TH1.h"
#include "TFile.h"

namespace o2
{
namespace TPC
{


class CalibTimeStructure : public CalibPedestal
{
  public:

    /// constructor
    CalibTimeStructure(PadSubset padSubset = PadSubset::ROC);

    /// default destructor
    ~CalibTimeStructure() override = default;

    /// read digits from GBTFrames
    static void readData(TString fileInfo);

    ProcessStatus writeDigits();

};
} //namespace TPC

} //namespace o2
#endif
