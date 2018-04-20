#if !defined(__CLING__) || defined(__ROOTCLING__)
  #include <iostream>

  #include "Rtypes.h"
  #include "TSystem.h"
  #include "TMath.h"
  #include "TString.h"
  #include "TStopwatch.h"
  #include "TGeoManager.h"

  #include "FairRunSim.h"
  #include "FairRuntimeDb.h"
  #include "FairPrimaryGenerator.h"
  #include "FairBoxGenerator.h"
  #include "FairParRootFileIo.h"
  #include "FairSystemInfo.h"

  #include "Field/MagneticField.h"

  #include "DetectorsPassive/Cave.h"
  #include "Generators/GeneratorFromFile.h"
  #include "TPCSimulation/Detector.h"
#endif

#define BOX_GENERATOR 1

void run_sim_tpc(Int_t nEvents = 1, TString mcEngine = "TGeant3")
{
  TString dir = getenv("VMCWORKDIR");
  TString geom_dir = dir + "/Detectors/Geometry/";
  gSystem->Setenv("GEOMPATH",geom_dir.Data());


  TString tut_configdir = dir + "/Detectors/gconfig";
  gSystem->Setenv("CONFIG_DIR",tut_configdir.Data());

  // Output file name
  char fileout[100];
  sprintf(fileout, "AliceO2_%s.tpc.mc_%i_event.root", mcEngine.Data(), nEvents);
  TString outFile = fileout;

  // Parameter file name
  char filepar[100];
  sprintf(filepar, "AliceO2_%s.tpc.params_%i.root", mcEngine.Data(), nEvents);
  TString parFile = filepar;

  // In general, the following parts need not be touched

  // Debug option
  gDebug = 0;

  // Timer
  TStopwatch timer;
  timer.Start();


  // Create simulation run
  FairRunSim* run = new FairRunSim();

  run->SetName(mcEngine);      // Transport engine
  run->SetOutputFile(outFile); // Output file
  FairRuntimeDb* rtdb = run->GetRuntimeDb();

  // Create media
  run->SetMaterials("media.geo"); // Materials

  // Create geometry
  o2::Passive::Cave* cave = new o2::Passive::Cave("CAVE");
  cave->SetGeometryFileName("cave.geo");
  run->AddModule(cave);

  auto magField = std::make_unique<o2::field::MagneticField>("Maps","Maps", -1., -1., o2::field::MagFieldParam::k5kG);
  run->SetField(magField.get());

  o2::field::MagneticField *magField = new o2::field::MagneticField("Maps","Maps", 0., -1., o2::field::MagFieldParam::k5kG);
  run->SetField(magField);


  // ===| Add TPC |============================================================
  o2::TPC::Detector* tpc = new o2::TPC::Detector(kTRUE);
  tpc->SetGeoFileName("TPCGeometry.root");
  run->AddModule(tpc);

  // Create PrimaryGenerator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
#ifdef BOX_GENERATOR
  FairBoxGenerator* boxGen = new FairBoxGenerator(211, 10); /*protons*/

  //boxGen->SetThetaRange(0.0, 90.0);
  boxGen->SetEtaRange(-0.9,0.9);
  boxGen->SetPRange(0.1, 5);
  boxGen->SetPhiRange(0., 360.);
  boxGen->SetDebug(kTRUE);

  primGen->AddGenerator(boxGen);

/*
  boxGenPi->SetThetaRange(89.5, 90.5);
  //boxGenPi->SetEtaRange(-0.9,0.9);
  boxGenPi->SetPRange(1, 1);
  boxGenPi->SetPhiRange(9., 11.);
  boxGenPi->SetBoxXYZ(-7.25*TMath::Sin(TMath::Pi()/18), 7.25*TMath::Cos(TMath::Pi()/18),
                      7.25*TMath::Sin(TMath::Pi()/18), 7.25*TMath::Cos(TMath::Pi()/18), 239);
  boxGenPi->SetDebug(kTRUE);

  primGen->AddGenerator(boxGenPi);
*/


//======================  void boxGen ==================================


  FairBoxGenerator* boxGenVoid = new FairBoxGenerator(211, 1);

  boxGenVoid->SetThetaRange(90.,90.);
  boxGenVoid->SetPRange(5, 5);
  boxGenVoid->SetPhiRange(170., 170.);
  boxGenVoid->SetBoxXYZ(0, 0, 0, 0, 247);
  boxGenVoid->SetDebug(kTRUE);

  primGen->AddGenerator(boxGenVoid);

//======================  Box at IROC sector 0 ============================

  FairBoxGenerator* boxGenPi = new FairBoxGenerator(211, 1); /*pi+*/

  boxGenPi->SetThetaRange(90.-0.24,90.+0.24);
  boxGenPi->SetPRange(2.,2.);
  boxGenPi->SetPhiRange(193.39-0.34,193.39+0.34);
//  boxGenPi->SetBoxXYZ(127.387, 30.93, 127.387, 30.93, 245);
  boxGenPi->SetBoxXYZ(127.8, 28.47, 126.8, 34.38, 249, 241);
  boxGenPi->SetDebug(kTRUE);

  primGen->SetBeam(0.,0.,0.,0.);
  primGen->SetBeamAngle(0.,0.,0.,0.);
  primGen->AddGenerator(boxGenPi);

//=========================================================================

#else
  // reading the events from a kinematics file (produced by AliRoot)
  auto extGen =  new o2::eventgen::GeneratorFromFile("Kinematics.root");
  extGen->SetStartEvent(2);
  primGen->AddGenerator(extGen);
#endif

  run->SetGenerator(primGen);

  // store track trajectories
  // run->SetStoreTraj(kTRUE);

  // Initialize simulation run
  run->Init();

  // Runtime database
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();

  // Start run
  run->Run(nEvents);
  delete run;
//  run->CreateGeometryFile("geofile_full.root");

  // Finish
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  // extract max memory usage
  FairSystemInfo sysinfo;

  std::cout << std::endl << std::endl;
  std::cout << "Macro finished succesfully." << std::endl;
  std::cout << "Output file is " << outFile << std::endl;
  std::cout << "Parameter file is " << parFile << std::endl;
  std::cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << std::endl << std::endl;
  std::cout << "Memory used " << sysinfo.GetMaxMemory() << "\n";
}
