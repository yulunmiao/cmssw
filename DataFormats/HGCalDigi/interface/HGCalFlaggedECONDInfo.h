#ifndef DataFormats_HGCalDigis_HGCalFlaggedECONDInfo_h
#define DataFormats_HGCalDigis_HGCalFlaggedECONDInfo_h

#include <vector>

class HGCalFlaggedECONDInfo
{
 public:

  enum FlagTypes { CBSTATUS=1,
                   HTBITS=2,
                   EBOBITS=4,
                   MATCHBIT=8,
                   TRUNCATED=16,
                   WRONGHEADERMARKER=32,
                   PAYLOADOVERFLOWS=64,
                   PAYLOADMISMATCHES=128};
  
  HGCalFlaggedECONDInfo() : HGCalFlaggedECONDInfo(0,0,0) {}
  HGCalFlaggedECONDInfo(uint32_t loc, uint32_t flagbits, uint32_t id)
    : iword(loc), flags(flagbits), eleid(id) {}
  HGCalFlaggedECONDInfo(const HGCalFlaggedECONDInfo &t)
    : HGCalFlaggedECONDInfo(t.iword,t.flags,t.eleid) {}
  
  bool cbFlag() { return flags & 0x1; };
  bool htFlag() { return (flags>>1) & 0x1; };
  bool eboFlag() { return (flags>>2) & 0x1; };
  bool matchFlag() { return (flags>>3) & 0x1; };
  bool truncatedFlag() { return (flags>>4) & 0x1; };
  bool wrongHeaderMarker() { return (flags>>5) & 0x1; };
  bool payloadOverflows() { return (flags>>6) & 0x1; };
  bool payloadMismatches() { return (flags>>7) & 0x1; };
    
  uint32_t iword,flags,eleid;
};

typedef std::vector<HGCalFlaggedECONDInfo> HGCalFlaggedECONDInfoCollection;

#endif
