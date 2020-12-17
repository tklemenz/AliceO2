//#include "Framework/WorkflowSpec.h"
//#include "Framework/DataSpecUtils.h"
//#include "Framework/Logger.h"
//#include "Framework/DeviceSpec.h"
//#include "Framework/ConfigParamSpec.h"
//#include "Framework/CompletionPolicyHelpers.h"
//#include "Framework/DispatchPolicy.h"
//#include "Framework/PartRef.h"
//#include "Framework/ConcreteDataMatcher.h"
//#include "DPLUtils/RootTreeReader.h"
//#include "DPLUtils/MakeRootTreeWriterSpec.h"
#include "TPCWorkflow/RecoWorkflow.h"
#include "TPCWorkflow/PublisherSpec.h"
#include "TPCWorkflow/TPCSectorCompletionPolicy.h"
//#include "DataFormatsTPC/TPCSectorHeader.h"
//#include "Algorithm/RangeTokenizer.h"
//#include "CommonUtils/ConfigurableParam.h"

//#include "DataFormatsTPC/Digit.h"
//#include "DataFormatsTPC/Constants.h"
//#include "DataFormatsTPC/ClusterGroupAttribute.h"
//#include "DataFormatsTPC/TrackTPC.h"
//#include "DataFormatsTPC/CompressedClusters.h"
//#include "DataFormatsTPC/ZeroSuppression.h"
//#include "DataFormatsTPC/Helpers.h"
//#include "DataFormatsTPC/ZeroSuppression.h"

//#include "Framework/DataProcessorSpec.h"
//#include "Framework/OutputSpec.h"
//#include "Framework/Output.h"

#include "SimulationDataFormat/IOMCTruthContainerView.h"
#include "SimulationDataFormat/ConstMCTruthContainer.h"
#include "SimulationDataFormat/MCCompLabel.h"

#include "TPCWorkflow/TrackReaderSpec.h"

//#include <string>
//#include <stdexcept>
//#include <unordered_map>
//#include <regex>

//#include <fstream>
//#include <sstream>
//#include <iomanip>
//#include <algorithm> // std::find
//#include <tuple>     // make_tuple
//#include <array>
//#include <gsl/span>

// we need a global variable to propagate the type the message dispatching of the
// publisher will trigger on. This is dependent on the input type
o2::framework::Output gDispatchTrigger{"", ""};

// Global variable used to transport data to the completion policy
o2::tpc::reco_workflow::CompletionPolicyData gPolicyData;

// add workflow options, note that customization needs to be declared before
// including Framework/runDataProcessing
void customize(std::vector<o2::framework::ConfigParamSpec>& workflowOptions)
{
  using namespace o2::framework;

  std::vector<ConfigParamSpec> options{
    {"input-type", VariantType::String, "clusters", {"clusters"}},
    {"dispatching-mode", VariantType::String, "prompt", {"determines when to dispatch: prompt, complete"}},
    {"disable-mc", VariantType::Bool, false, {"disable sending of MC information"}}};

  std::swap(workflowOptions, options);
}

// customize dispatch policy, dispatch immediately what is ready
void customize(std::vector<o2::framework::DispatchPolicy>& policies)
{
  using DispatchOp = o2::framework::DispatchPolicy::DispatchOp;
  // we customize all devices to dispatch data immediately
  auto readerMatcher = [](auto const& spec) {
    return std::regex_match(spec.name.begin(), spec.name.end(), std::regex(".*-reader"));
  };
  auto triggerMatcher = [](auto const& query) {
    // a bit of a hack but we want this to be configurable from the command line,
    // however DispatchPolicy is inserted before all other setup. Triggering depending
    // on the global variable set from the command line option. If scheduled messages
    // are not triggered they are sent out at the end of the computation
    return gDispatchTrigger.origin == query.origin && gDispatchTrigger.description == query.description;
  };
  policies.push_back({"prompt-for-reader", readerMatcher, DispatchOp::WhenReady, triggerMatcher});
}

#include "Framework/runDataProcessing.h" // the main driver

using namespace o2::framework;

/// MC info is processed by default, disabled by using command line option `--disable-mc`
///
/// This function hooks up the the workflow specifications into the DPL driver.
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec specs;

  auto inputType = cfgc.options().get<std::string>("input-type");
  auto dispmode = cfgc.options().get<std::string>("dispatching-mode");
  bool doMC = not cfgc.options().get<bool>("disable-mc");

  if (inputType == "clusters") {

    // We provide a special publishing method for labels which have been stored in a split format and need
    // to be transformed into a contiguous shareable container before publishing. For other branches/types this returns
    // false and the generic RootTreeWriter publishing proceeds
    static RootTreeReader::SpecialPublishHook hook{[](std::string_view name, ProcessingContext& context, o2::framework::Output const& output, char* data) -> bool {
    if (TString(name.data()).Contains("TPCDigitMCTruth") || TString(name.data()).Contains("TPCClusterHwMCTruth") || TString(name.data()).Contains("TPCClusterNativeMCTruth")) {
      auto storedlabels = reinterpret_cast<o2::dataformats::IOMCTruthContainerView const*>(data);
      o2::dataformats::ConstMCTruthContainer<o2::MCCompLabel> flatlabels;
      storedlabels->copyandflatten(flatlabels);
      LOG(INFO) << "PUBLISHING CONST LABELS " << flatlabels.getNElements();
      context.outputs().snapshot(output, flatlabels);
      return true;
    }
    return false;
    }};
    
    std::vector<int> tpcSectors(36);
    std::iota(tpcSectors.begin(), tpcSectors.end(), 0);
    std::vector<int> laneConfiguration = tpcSectors;

    if (dispmode == "complete") {
      // nothing to do we leave the matcher empty which will suppress the dispatch
      // trigger and all messages will be sent out together at end of computation
    } else if (inputType == "clusters") {
      gDispatchTrigger = o2::framework::Output{"TPC", "CLUSTERNATIVE"};
    }

    specs.emplace_back(o2::tpc::getPublisherSpec(o2::tpc::PublisherConf{
                                                     "tpc-native-cluster-reader",
                                                     "tpcrec",
                                                     {"clusterbranch", "TPCClusterNative", "Branch with TPC native clusters"},
                                                     {"clustermcbranch", "TPCClusterNativeMCTruth", "MC label branch"},
                                                     OutputSpec{"TPC", "CLUSTERNATIVE"},
                                                     OutputSpec{"TPC", "CLNATIVEMCLBL"},
                                                     tpcSectors,
                                                     laneConfiguration,
                                                     &hook},
                                                     doMC));

    return std::move(specs);
  }

  else if ((inputType == "tracks")) {
    if (dispmode == "complete") {
      // nothing to do we leave the matcher empty which will suppress the dispatch
      // trigger and all messages will be sent out together at end of computation
    } else if (inputType == "tracks") {
      gDispatchTrigger = o2::framework::Output{"TPC", "TRACKS"};
    }

  specs.push_back(o2::tpc::getTPCTrackReaderSpec(doMC));

  return std::move(specs);
  }
}