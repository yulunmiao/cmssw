#! /usr/bin/env python3
# Author: Izaak Neutelings (June 2023)
# Description: Write simple YAML file with dummy value for testing HGCalConfigESSourceFromYAML
import os, yaml


def b2h(x):
  """
  Convert binary to hex.
  E.g b2h(4) -> '0x4', b2h('100') -> '0x4'
  """
  return hex(x if isinstance(x,int) else int(x,2)) 
  

def getmapper(econids,dname='',run=0):
  """Create mapper to YAML config files."""
  # https://github.com/CMS-HGCAL/cmssw/blob/hgcal-condformat-HGCalNANO-13_2_0_pre2/Geometry/HGCalMapping/data/modulelocator_tb.txt
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
  

def getconfig(rocs):
  """Create YAML config file."""
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
  

def main():
  econids = [ # ECON IDs for mapper
    '0x000',
    '0x400',
  ]
  rocs = [
    0, 1, 2,
  ]
  
  # CONFIGs
  outfname = "test_hgcal_yamlconfig.yaml"
  config = getconfig(rocs)
  print(f">>> Writing config {outfname}...")
  with open(outfname,'w') as outfile:
    yaml.dump(config,outfile,sort_keys=False)
  
  # MAPPER
  mapper = getmapper(econids)
  outfname = "test_hgcal_yamlmapper.yaml"
  print(f">>> Writing mapper {outfname}...")
  with open(outfname,'w') as outfile:
    yaml.dump(mapper,outfile,sort_keys=False)
  

if __name__=='__main__':
  main()
  
