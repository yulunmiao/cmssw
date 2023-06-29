#include "RecoLocalCalo/HGCalRecAlgos/interface/HGCalCalibrationParameterProvider.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

HGCalCalibrationParameterProvider::HGCalCalibrationParameterProvider(HGCalCalibrationParameterProviderConfig config):
        config_(config),
        calibrationParameter_(config_.EventSLinkMax*config_.sLinkCaptureBlockMax*config_.captureBlockECONDMax*config_.econdERXMax*config_.erxChannelMax) {
                  LogDebug("HGCalCalibrationParamter") << calibrationParameter_.size();
        }

const uint32_t HGCalCalibrationParameterProvider::denseMap(uint32_t ElectronicsID) const{
    uint32_t sLink = ((ElectronicsID >> kFEDIDShift) & kFEDIDMask);
    uint32_t captureBlock = ((ElectronicsID >> kCaptureBlockShift) & kCaptureBlockMask);
    uint32_t econd = ((ElectronicsID >> kECONDIdxShift) & kECONDIdxMask);
    uint32_t eRx = ((ElectronicsID >> kECONDeRxShift) & kECONDeRxMask);
    uint32_t channel = ((ElectronicsID >> kHalfROCChannelShift) & kHalfROCChannelMask);
    uint32_t rtn = sLink * config_.sLinkCaptureBlockMax + captureBlock;
    rtn = rtn * config_.captureBlockECONDMax + econd;
    rtn = rtn * config_.econdERXMax + eRx;
    rtn = rtn * config_.erxChannelMax + channel;
    return rtn;
}

CalibrationParameter& HGCalCalibrationParameterProvider::operator[](uint32_t ElectronicsID){
    return calibrationParameter_[denseMap(ElectronicsID)];
}
