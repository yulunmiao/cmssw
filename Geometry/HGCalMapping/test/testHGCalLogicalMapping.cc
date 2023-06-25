#include "Geometry/HGCalMapping/interface/HGCalSiPMCellLocator.h"
#include "Geometry/HGCalMapping/interface/HGCalModuleLocator.h"

#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>

#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/Utilities/interface/EDMException.h"


void testSiPMCellLocator(int nentries, std::string path_channelmap, std::string path_modulemap)
{
  std::cout << "Testing of HGCalSiPMCellLocator class" << std::endl;

  HGCalSiPMCellLocator celllocator;
  celllocator.buildLocatorFrom(path_channelmap);

  int zside;
  int plane,modiu,modiv,plane_,modiu_,modiv_,isSiPM,isHD,modType,z(0),maxseq(20);
  int econdidx,captureblock,slink,captureblockidx,fedid,econderx(0),halfrocch(0),seq;
  std::string DAQ;

  edm::FileInPath fip(path_modulemap);
  std::ifstream file(fip.fullPath());
  std::string line;

  auto start = std::chrono::high_resolution_clock::now();

  if (file.is_open())
  {
    std::getline(file, line);
    for (int i=0; i < nentries; i++)
    {
      std::getline(file, line);
      std::istringstream stream(line);
      stream >> plane >> modiu >> modiv >> isSiPM >> isHD >> modType >> econdidx >> captureblock >> slink >> captureblockidx >> fedid >> DAQ >> zside;
      if(isSiPM)
      {
        for(seq = 0; seq < maxseq; seq++) {

          // Calibration and common mode channels
          if (seq == 8 || seq == 17 || seq == 18) continue;

          HGCalElectronicsId eid(zside>0,fedid, captureblockidx, econdidx, econderx, halfrocch);                 
          
          HGCScintillatorDetId detid = celllocator.getDetId(eid, seq, z, plane, modiu, modiv);
          assert(detid.sipm());
          assert(!std::isnan(detid.iphi()));
          assert(!std::isnan(detid.ring()));
          assert(detid.ring()>0 && detid.layer()>0);
          assert(detid.iphi()<=360);

          std::tuple celllocation = celllocator.getCellLocation(eid, seq, plane, modiu, modiv);
          assert(std::get<0>(celllocation) == abs(detid.ring()));
          assert(std::get<1>(celllocation) == detid.iphi()%10);

          std::tie(plane_,modiu_,modiv_) = celllocator.getModuleLocation(detid);
          assert(plane_ == plane);
          assert(modiu_ == modiu);
        }
      }
    }
  }
  else
  {
    edm::Exception e(edm::errors::FileOpenError, "HGCalMappingTest::testSiPMCellLocator : module mapping file can not be found.");
    throw e;
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

  std::cout << "HGCalSiPMCellLocator done" << std::endl;
  std::cout << "Test for  " << nentries << "  tileboards with  " << maxseq << "  tiles each finished after  " << duration.count() << "  microseconds" << std::endl;
}

int main(int argc, char** argv) {

    if (argc<3) {
        std::cout << "Usage: HGCalMappingTest n_entries path_to_channels_map path_to_module_map" << std::endl;
        return -1;
    }
    int nentries(atoi(argv[1]));
    std::string channelsmap(argv[2]);
    std::string modulemap(argv[3]);
    testSiPMCellLocator(nentries, channelsmap, modulemap);
    return 0;
}
