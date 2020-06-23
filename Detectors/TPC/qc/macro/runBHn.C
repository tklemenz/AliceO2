#include <boost/histogram.hpp>
#include <boost/histogram/ostream.hpp>
#include <boost/histogram/algorithm/project.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <sstream>

#include <TPCBase/BHn.h>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

using namespace o2::tpc;
using namespace boost::histogram;
//using namespace std::literals; // enables _c suffix

std::string demangle(const char* name)
{
  int status = -4; // some arbitrary value to eliminate the compiler warning
  std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};
  return (status == 0) ? res.get() : name;
}

void runBHn()
{
  printf("\e[1m\n\n\n2D Histogram\n\n\e[0m");
  BHn twoDim{2,2,0,2};

  auto& histo = twoDim.getHisto();

  std::vector<double> xy[2] = {{0.5, 1.5}, {1.5, 0.5}};

  twoDim.getHisto()(0.5,1.5);

  histo.fill(xy);
  //histo(0.5, 1.5);

  /*auto objectType = demangle(typeid(twoDim).name());
  auto histoType = demangle(typeid(histo).name());

  printf("\n\ntype of class object: %s", objectType.c_str());
  printf("\n\ntype of histogram:\n%s\n\n", histoType.c_str());
  
  //std::cout<<"Adress of histo: "<<&histo<<endl;
*/
  std::ostringstream os;
  for (auto x : indexed(histo, coverage::all)) {
    const auto i = x.index(0); // current index along first axis
    const auto j = x.index(1); // current index along second axis
    const auto v = *x;         // "dereference" to get the bin value
    if(v!=0) {
      os << boost::format("%i %i: %i\n") %i %j %v;
    }
    
  }
  std::cout << os.str() << std::flush;




  // another one

  printf("\e[1m\n\n\n3D Histogram\e[0m\n\n");

  BHn multiDim{3,3,0,3};

  auto& histo2 = multiDim.getHisto();

  auto nAxes = multiDim.getNAxes();

  std::vector<double> xyz[3] = {{0.5, 1.5}, {1.5, 0.5}, {2, 2.5}};

  histo2.fill(xyz);
  //histo2(0.5,1.5,2.5);

  std::ostringstream os2;

  for (auto x : indexed(histo2, coverage::all)) {
    const auto i = x.index(0); // current index along first axis
    const auto j = x.index(1); // current index along second axis
    const auto k = x.index(2); // current index along third axis
    const auto v = *x;         // "dereference" to get the bin value

    if (v!=0) {
      os2 << boost::format("%i %i %i: %i\n") %i %j %k %v;
    }
  }
  std::cout << os2.str() << std::flush;



  // another one

  printf("\e[1m\n\n\n5D Histogram\e[0m\n\n");
  int Axes = 5;
  std::vector<int> bins;
  std::vector<float> begin;
  std::vector<float> end;
  std::vector<std::string> name{"a","b","c","d","e"};

  for (int i=0; i<Axes; i++) {
    //float binVal = rand() % 100 + 10;
    //float beginVal = rand() % 100;
    //float endVal = rand() % 100 + 1000;
    bins.emplace_back(10);
    begin.emplace_back(0);
    end.emplace_back(10);
    //std::cout<<name.at(i)<<std::endl;
    //std::cout<<"bin: "<< binVal << " begin: "<<beginVal<<" end: "<<endVal<<std::endl; 
  }

  //std::cout<<"axes: "<< Axes << " bins: "<<bins.size()<<" begins: "<<begin.size()<<" ends: "<<end.size()<< " names: "<<name.size() << std::endl; 

  BHn fiveD(Axes, bins, begin, end, name);

  auto& fiveDHisto = fiveD.getHisto();

  //for (int i=0; i<100; i++) {
  //  fiveDHisto(rand()%10, rand()%10, rand()%10, rand()%10, rand()%10);
  //}


  std::vector<double> multiDimData[5] = {{3,3,4,4,4,4,5,5,5,5,5,5,6,6,6,6,7,7},{0,3,4,9,4,7,8,1,5,7,0,0,0,0,0,0,0,0},{0,9,3,1,3,5,7,9,0,4,0,0,0,0,0,0,0,0}
                                        ,{0,6,7,7,6,5,4,3,2,3,0,0,0,0,0,0,0,0},{0,8,3,9,8,0,9,8,9,7,0,0,0,0,0,0,0,0}};
  fiveDHisto.fill(multiDimData);

  std::ostringstream os3;

  for (auto x : indexed(fiveD.getHisto(), coverage::all)) {
    const auto i = x.index(0); // current index along first axis
    const auto j = x.index(1); // current index along second axis
    const auto k = x.index(2); // current index along third axis
    const auto l = x.index(3); // current index along third axis
    const auto m = x.index(4); // current index along third axis
    const auto v = *x;         // "dereference" to get the bin value

    if (v!=0) {
      os3 << boost::format("%i %i %i %i %i: %i\n") %i %j %k %l %m %v;
    }
  }
  os3<<"\n\n";
  std::cout << os3.str() << std::flush;

  std::vector<int> binsVec;

  fiveD.getNBins(binsVec);

  std::vector<int> iter{0};
  std::vector<int> iter2{0,1};

  //auto projectA = algorithm::project(fiveDHisto, iter);             // this line breaks the macro
  //auto projectA = fiveDHisto.project(0,1);

  for (unsigned int i=0; i<binsVec.size(); i++) {
    std::cout<<"Axis "<<i<<" has " << binsVec.at(i) << " bins." << std::endl;
  }

  //std::stringstream osProjectA;
  //osProjectA << projectA;
  //std::cout << osProjectA.str() << std::endl;

  auto projectA = algorithm::project(fiveDHisto, iter);
  auto projectB = algorithm::project(fiveDHisto, iter2);

  //auto projectB = algorithm::project(fiveDHisto, 0_c);

  for (unsigned int i=0; i<binsVec.size(); i++) {
    std::cout<<"Axis "<<i<<" has " << binsVec.at(i) << " bins." << std::endl;
  }

  std::cout<<std::endl<<std::endl;

  for (auto&& x : indexed(projectA)) {
    //printf("bin: %d, content: %d\n", x.index(0),*x);
    std::cout<<"bin: "<<x.index(0)<<", content: "<<*x<<std::endl;
  }

  std::stringstream osProjectA;
  osProjectA << "\n\n";
  osProjectA << projectA;
  std::cout << osProjectA.str() << std::endl;


  TH1F* projection1 = fiveD.getTH1(0);
  TH2F* projection2 = fiveD.getTH2(0,1);
  TH2F* projection3 = fiveD.getTH2(1,0);

  auto canvas1 = new TCanvas("c1","1D projection");
  projection1->Draw("HIST");
  
  auto canvas2 = new TCanvas("c2","2D projection");
  projection2->Draw("colz");

  auto canvas3 = new TCanvas("c3","2D projection");
  projection3->Draw("colz");






  // another one

  printf("\e[1m\n\n\n9D Histogram\e[0m\n\n");
  int Axes2 = 9;
  std::vector<int> bins2;
  std::vector<float> begin2;
  std::vector<float> end2;

  for (int i=0; i<Axes2; i++) {
    bins2.emplace_back(10);
    begin2.emplace_back(0);
    end2.emplace_back(10);
  }

  BHn nineD(Axes2, bins2, begin2, end2);

  auto& nineDHisto = nineD.getHisto();

  std::vector<double> nineDimData[9] = {{0},{0},{0},{0},{5},{0},{0},{0},{0}};//,{10}};//,{0},{0},{0},{0},{15},{0},{0},{0},{0},{20}};

  nineDHisto.fill(nineDimData);

  std::ostringstream os4;

  /*for (auto x : indexed(nineD.getHisto(), coverage::all)) {
    const auto v = *x;         // "dereference" to get the bin value
    if (v!=0) {
      os4 << boost::format("%i\n") %v;
    }
  }
  os4<<"\n\n";
  std::cout << os4.str() << std::flush;*/
}
