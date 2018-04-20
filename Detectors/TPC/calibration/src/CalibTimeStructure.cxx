#include "TPCCalibration/CalibTimeStructure.h"

using namespace o2::TPC;

CalibTimeStructure::CalibTimeStructure(PadSubset padSubset)
  : CalibPedestal()
{}

//____________________________________________________________________________________________
void CalibTimeStructure::readData(TString fileInfo)
{
  CalibTimeStructure calibStruct;
  calibStruct.setupContainers(fileInfo);

  calibStruct.processEvent();

  for (Int_t i=0; i<1000; ++i) {
    if (calibStruct.processEvent() != CalibRawBase::ProcessStatus::Ok) break;
  }

  calibStruct.writeDigits();

}

//____________________________________________________________________________________________
CalibTimeStructure::ProcessStatus CalibTimeStructure::writeDigits()
{

  std::cout << std::endl << "entering writeDigits()\n";
  std::cout << std::endl << "mGBTFrameContainers: " << mGBTFrameContainers.size() << std::endl;
  if (!mGBTFrameContainers.size()) return ProcessStatus::NoReaders;
  resetEvent();

  std::cout << std::endl << "task ist noch da\n";

  //loop over raw readers, fill digits for 500 time bins and process digits

  ProcessStatus status = ProcessStatus::Ok;

  TH1D *hSignalChannel = new TH1D("hSignalChannel",";signal [ADC];counts",1000,0,1000);

  std::cout << std::endl << "task ist noch da 2\n";

  std::vector<Digit> digits(80);

  std::cout << std::endl << "task ist noch da 3\n";

  for (auto& reader_ptr : mGBTFrameContainers) {
    auto reader = reader_ptr.get();
    int readTimeBins = 0;
    for (int i=0; i<mTimeBinsPerCall; ++i) {
      std::cout << std::endl << "task ist noch da 4\n";
      std::cout << reader->getData(digits);

      if (reader->getData(digits)) {
        std::cout << std::endl << "task ist noch da 5\n";

        for (auto& digi : digits) {
          if (digi.getRow() == 20 && digi.getPad() == 55) {
            hSignalChannel->Fill(digi.getChargeFloat());
          }
        }
        std::cout << std::endl << "read time bins\n" <<readTimeBins;

        ++readTimeBins;
      }
      digits.clear();
    }
    if (status == ProcessStatus::Ok) {
      if (readTimeBins == 0 ) {
        return ProcessStatus::NoMoreData;
      }
      else if (readTimeBins < mTimeBinsPerCall) {
        status = ProcessStatus::Truncated;
      }
    }
  }
  std::cout << std::endl << "task ist noch da\n";

  endEvent();
  ++mNevents;

  std::cout << std::endl << "about to write file\n";

  TFile *Outfile = new TFile("/home/tom/myana/Results/RawDataTesting/testSignalReadOut.root","recreate");
  Outfile->WriteObject(hSignalChannel, "hSignalChannel");
  delete Outfile;
  delete hSignalChannel;
  std::cout << std::endl << "done writeDigits()\n";

  return status;

}
