#ifndef RecoLocalCalo_HGCalRecAlgos_HGCalCalibrationParameterProvider
#define RecoLocalCalo_HGCalRecAlgos_HGCalCalibrationParameterProvider

#include <cstdint>
#include <vector>

struct HGCalCalibrationParameterProviderConfig {
    uint32_t EventSLinkMax{1000};         ///< maximum number of S-Links in one Event
    uint32_t sLinkCaptureBlockMax{10};    ///< maximum number of capture blocks in one S-Link
    uint32_t captureBlockECONDMax{12};    ///< maximum number of ECON-Ds in one capture block
    uint32_t econdERXMax{12};             ///< maximum number of eRxs in one ECON-D
    uint32_t erxChannelMax{37};           ///< maximum number of channels in one eRx

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

    constexpr uint32_t denseMap(uint32_t ElectronicsID) const{
        uint32_t sLink = ((ElectronicsID >> kFEDIDShift) & kFEDIDMask);
        uint32_t captureBlock = ((ElectronicsID >> kCaptureBlockShift) & kCaptureBlockMask);
        uint32_t econd = ((ElectronicsID >> kECONDIdxShift) & kECONDIdxMask);
        uint32_t eRx = ((ElectronicsID >> kECONDeRxShift) & kECONDeRxMask);
        uint32_t channel = ((ElectronicsID >> kHalfROCChannelShift) & kHalfROCChannelMask);
        uint32_t rtn = sLink * sLinkCaptureBlockMax + captureBlock;
        rtn = rtn * captureBlockECONDMax + econd;
        rtn = rtn * econdERXMax + eRx;
        rtn = rtn * erxChannelMax + channel;
        return rtn;
    }
};

#endif

// struct CalibrationParameter{
//     float pedestal, cm_slope, cm_offset, kappa_bxm1;
// };
//
// class HGCalCalibrationParameterProvider{
//
//     private:
//     enum HGCalElectronicsIdMask {
//         kZsideMask = 0x1,
//         kFEDIDMask = 0x3ff,
//         kCaptureBlockMask = 0xf,
//         kECONDIdxMask = 0xf,
//         kECONDeRxMask = 0xf,
//         kHalfROCChannelMask = 0x3f
//     };
//     enum HGCalElectronicsIdShift {
//         kZsideShift = 28,
//         kFEDIDShift = 18,
//         kCaptureBlockShift = 14,
//         kECONDIdxShift = 10,
//         kECONDeRxShift = 6,
//         kHalfROCChannelShift = 0
//     };
//     HGCalCalibrationParameterProviderConfig config_;
//     std::vector<CalibrationParameter> calibrationParameter_;
//     const uint32_t denseMap(uint32_t ElectronicsID) const;
//
//     public:
//     explicit HGCalCalibrationParameterProvider(){};
//     void initialize(HGCalCalibrationParameterProviderConfig config);
//     CalibrationParameter& operator[](uint32_t ElectronicsID);
// };
