#! /usr/bin/env python3
# Author: Izaak Neutelings (June 2023)
# Description: Write simple YAML file with dummy value for testing HGCalConfigESSourceFromYAML
import yaml

def main():
  channels = [
    0, 1, #2, 3, 4, 5, 6, 7, 8, 9,
    #10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    #20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    #30, 31, 32, 33, 34, 35, 36, 37, 38,
    #64, 65, 66, 67, 68, 69,
    #70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    #80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    #90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
    #100, 101, 102,
    #128, 129,
    #130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
    #140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
    #150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    #160, 161, 162, 163, 164, 165, 166,
    #192, 193, 194, 195, 196, 197, 198, 199,
    #200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
    #210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
    #220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
    #256, 257, 258, 259,
    #260, 261, 262, 263, 264, 265, 266, 267, 268, 269,
    #270, 271, 272, 273, 274, 275, 276, 277, 278, 279,
    #280, 281, 282, 283, 284, 285, 286, 287, 288, 289,
    #290, 291, 292, 293, 294,
    #320, 321, 322, 323, 324, 325, 326, 327, 328, 329,
    #330, 331, 332, 333, 334, 335, 336, 337, 338, 339,
    #340, 341, 342, 343, 344, 345, 346, 347, 348, 349,
    #350, 351, 352, 353, 354, 355, 356, 357, 358, 
  ]
  rocs = [ # hexadecimal ElectronsId
    hex(c) for c in channels
  ]
  halves = [ 0, 1 ] # channel halves
  
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
  for channel in channels:
    rocname = f"roc_s{channel}"
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
  
  outfname = "test_hgcal_yamlconfig.yaml"
  print(f">>> Writing {outfname}...")
  with open(outfname,'w') as outfile:
    yaml.dump(config,outfile,sort_keys=False)
  

if __name__=='__main__':
  main()
  
