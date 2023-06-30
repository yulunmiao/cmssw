#! /usr/bin/env python3
# Author: Izaak Neutelings (June 2023)
# Description:
#   Write simple YAML file with dummy value for testing HGCalConfigESSourceFromYAML
# Instructions:
#   ./hgcal_yamlconfig_writer.py
#   ./hgcal_yamlconfig_writer.py -m $CMSSW_BASE/src/Geometry/HGCalMapping/data/modulelocator_tb.txt
# References:
#   https://github.com/CMS-HGCAL/cmssw/blob/hgcal-condformat-HGCalNANO-13_2_0_pre2/Geometry/HGCalMapping/data/modulelocator_tb.txt
#   https://github.com/CMS-HGCAL/cmssw/blob/hgcal-condformat-HGCalNANO-13_2_0_pre2/Geometry/HGCalMapping/test/prepareLogicalYAMLMapper.cc
import os, yaml
#import ROOT; ROOT.PyConfig.IgnoreCommandLineOptions = True

def b2h(x):
  """
  Convert binary to hex.
  E.g b2h(4) -> '0x4', b2h('100') -> '0x4'
  """
  return hex(x if isinstance(x,int) else int(x,2)) 
  

def getmapper(econids,dname='',run=0,verb=0):
  """Create mapper to YAML config files."""
  if verb>=1:
    print(f">>> getmapper({econids!r})")
  mapper = {
    'run': 0,
    'ECONs': [
      { 'id': econ,
        'configs': {
          'ECON': os.path.join(dname,f"Run_{run}.yaml"),
          'ROCs': os.path.join(dname,"initial_full_config.yaml"),
        }
      } for econ in econids
    ]
  }
  return mapper
  

def getconfig(rocs,verb=0):
  """Create YAML config file."""
  if verb>=1:
    print(f">>> getconfig({rocs!r})")
  halves = [ 0, 1 ] # ROC halves
  config = {
    'metaData': {
       ###'Channel_off': [],
       'characMode': 1,
       'myfloat': 0.1234567890, # for testing parser
       ###'chip_params': {},
       ###'hexactrl': 'hc640232',
       ###'hw_type': 'LD',
       ###'keepRawData': 1,
       ###'keepSummary': 1,
       ###'selTC4': 1,
       ###'testName': 'pedestal_run',
    },
    'client': {
      'outputDirectory': '/home/daq/hexactrl-script/data/module822-toa400/pedestal_run/run_20230323_174847/',
      'rocDataOutputFormat': 'root',
      'run_type': 'pedestal_run',
    },
  }
  for roc in rocs:
    rocname = f"roc_s{roc}"
    config[rocname] = {
      'sc': {
        'DigitalHalf': {
          h: { 'CalibrationSC': 1, } for h in halves
        },
        'GlobalAnalog': {
          h: { 'CalibrationSC': 1, } for h in halves
        },
      },
    }
  return config
  

def getmodule(filename,verb=0):
  """Get modules from input file. Python equivalent of
  Geometry/HGCalMapping/test/prepareLogicalYAMLMapper.cc"""
  if verb>=1:
    print(f">>> getmodule({filename!r})")
    print(f">>>   Importing ROOT, HGCalModuleLocator, HGCalElectronicsId...")
  from ROOT import gSystem, gROOT
  #gSystem.Load(f"{os.environ['CMSSW_BASE']}/lib/{os.environ['SCRAM_ARCH']}/libGeometryHGCalMapping.so")
  gROOT.ProcessLine('#include "Geometry/HGCalMapping/interface/HGCalModuleLocator.h"')
  from ROOT import HGCalModuleLocator, HGCalElectronicsId
  print(HGCalElectronicsId(0x04a4c3ff).rawId())
  locator = HGCalModuleLocator()
  locator.buildLocatorFrom(filename,True)
  econids = [ ]
  if verb>=1:
    print(f">>>   Looping over modules...")
  modinfo = locator.getInfo()
  for module in modinfo.params_:
    elecid = HGCalElectronicsId(module.zside,module.fedid,module.captureblock,module.econdidx,0,0).raw()
    if verb>=1:
      print(f">>>   elecid={elecid} -> {hex(elecid)!r}")
    econids.append(hex(elecid))
  return econids
  

def main(args):
  
  # SETTINGS
  verbosity = args.verbosity
  modulemap = args.modulemap
  
  # MODULES 
  if modulemap:
    econids = getmodule(modulemap,verb=verbosity)
  else:
    econids = [ # ECON IDs for mapper
      '0x000',
      '0x400',
    ]
  
  rocs = [
    0, 1, 2,
  ]
  
  # CONFIGs
  outfname = "test_hgcal_yamlconfig.yaml"
  config = getconfig(rocs,verb=verbosity)
  print(f">>> Writing config {outfname} for {len(rocs)} ROCs...")
  with open(outfname,'w') as outfile:
    yaml.dump(config,outfile,sort_keys=False)
  
  # MAPPER
  mapper = getmapper(econids,verb=verbosity)
  outfname = "test_hgcal_yamlmapper.yaml"
  print(f">>> Writing mapper {outfname} for {len(econids)} ECON modules...")
  with open(outfname,'w') as outfile:
    yaml.dump(mapper,outfile,sort_keys=False)
  

if __name__=='__main__':
  from argparse import ArgumentParser
  description = '''This script make creates YAML configuration file for (test) reconstructing raw HGCAL data.'''
  parser = ArgumentParser(prog='hgcal_yamlconfig_writer',description=description,epilog="Good luck!")
  parser.add_argument('-m', "--modulemap", help="Input file with map for HGCalModuleLocator, e.g. Geometry/HGCalMapping/data/modulelocator_tb.txt" )
  parser.add_argument('-v', "--verbose", dest='verbosity', type=int, nargs='?', const=1, default=0,
                                         help="set level of verbosity, default=%(default)s" )
  args = parser.parse_args()
  main(args)
