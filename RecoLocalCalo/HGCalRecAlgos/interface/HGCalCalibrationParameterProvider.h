#ifndef RecoLocalCalo_HGCalRecAlgos_HGCalCalibrationParameterProvider
#define RecoLocalCalo_HGCalRecAlgos_HGCalCalibrationParameterProvider

#include <cstdint>
#include <vector>

struct HGCalCalibrationParameterProviderConfig {
    uint32_t EventSLinkMax{1000};              ///< maximum number of S-Links in one Event
    uint32_t sLinkCaptureBlockMax{10};    ///< maximum number of capture blocks in one S-Link
    uint32_t captureBlockECONDMax{12};    ///< maximum number of ECON-Ds in one capture block
    uint32_t econdERXMax{12};             ///< maximum number of eRxs in one ECON-D
    uint32_t erxChannelMax{37};           ///< maximum number of channels in one eRx
};

struct CalibrationParameter{
    float pedestal, cm_slope, cm_offset, kappa_bxm1;
}; 

class HGCalCalibrationParameterProvider{

    private:
    enum HGCalElectronicsIdMask {
        kZsideMask = 0x1,
        kFEDIDMask = 0x3ff,
        kCaptureBlockMask = 0xf,
        kECONDIdxMask = 0xf,
        kECONDeRxMask = 0xf,
        kHalfROCChannelMask = 0x3f
    };
    enum HGCalElectronicsIdShift {
        kZsideShift = 28,
        kFEDIDShift = 18,
        kCaptureBlockShift = 14,
        kECONDIdxShift = 10,
        kECONDeRxShift = 6,
        kHalfROCChannelShift = 0
    };
    HGCalCalibrationParameterProviderConfig config_;
    std::vector<CalibrationParameter> calibrationParameter_;
    const uint32_t denseMap(uint32_t ElectronicsID) const;
    
    public:
    explicit HGCalCalibrationParameterProvider(HGCalCalibrationParameterProviderConfig config);
    CalibrationParameter& operator[](uint32_t ElectronicsID);
};

#endif