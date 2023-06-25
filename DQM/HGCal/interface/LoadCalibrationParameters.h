#ifndef __LoadCalibrationParameters__
#define __LoadCalibrationParameters__

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "TString.h"

class CalibrationParameterLoader {
public:
    CalibrationParameterLoader();
    void loadParameters();

    std::map<int, double> map_pedestals;
    std::map<int, std::vector<double> > map_cm_parameters;

private:
    TString csv_file_name;
}; // CalibrationParameterLoader

CalibrationParameterLoader::CalibrationParameterLoader() {
    csv_file_name = "./meta_conditions/calibration_parameters.csv";
}

void CalibrationParameterLoader::loadParameters() {
    printf("[INFO] CalibrationParameterLoader: Load calibration parameters: %s\n", csv_file_name.Data());

    std::string line;
    std::ifstream loaded_csv_file(csv_file_name.Data());

    if(loaded_csv_file.is_open()) {
        while(getline(loaded_csv_file, line)) {
            // skip comments
            if(line.find("#")!=std::string::npos) continue;

            std::size_t found_1st_index = line.find(",");
            std::size_t found_2nd_index = line.find(",", found_1st_index+1, 1);
            std::size_t found_3rd_index = line.find(",", found_2nd_index+1, 1);
            std::size_t found_4th_index = line.find(",", found_3rd_index+1, 1);

            int eleId   = std::stoi( line.substr(0,found_1st_index) );
            double pedestal  = std::stod( line.substr(found_1st_index+1, found_2nd_index) );
            double slope     = std::stod( line.substr(found_2nd_index+1, found_3rd_index) );
            double intercept = std::stod( line.substr(found_3rd_index+1, found_4th_index) );

            std::vector<double> v = {slope, intercept};
            map_pedestals[eleId] = pedestal;
            map_cm_parameters[eleId] = v;

            printf("eleId = %d, pedestal = %.3f, slope = %.3f, intercept = %6.3f\n",
                    eleId, map_pedestals[eleId], map_cm_parameters[eleId][0], map_cm_parameters[eleId][1] );
        }
        loaded_csv_file.close();
    } else {
        std::cout << "[ERROR] unable to open " << csv_file_name.Data() << std::endl;
    }
}

//
// Format of csv calibration parameters
//
// #--------------------------------------------------------------------------------------------------------------------------------------------
// # channel, pedestal, slope, intercept, kappa, charge, residual_offset, conversion_ADC_to_fC, conversion_ToT_to_fC, conversion_ToA_to_ns
// #--------------------------------------------------------------------------------------------------------------------------------------------
// 0,87.95,0.23,1.34,0.,0.,0.,0.,0.,0.
// 1,87.45,0.22,0.48,0.,0.,0.,0.,0.,0.
// 2,87.01,0.26,1.06,0.,0.,0.,0.,0.,0.
// 3,84.50,0.26,1.15,0.,0.,0.,0.,0.,0.
// 4,86.62,0.29,1.07,0.,0.,0.,0.,0.,0.
// 5,88.59,0.30,1.14,0.,0.,0.,0.,0.,0.
// 6,86.17,0.29,1.16,0.,0.,0.,0.,0.,0.
// ...
//
#endif // __LoadCalibrationParameters__
