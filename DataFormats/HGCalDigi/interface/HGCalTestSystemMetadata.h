#ifndef DataFormats_HGCalDigis_HGCalTestSystemMetaData_h
#define DataFormats_HGCalDigis_HGCalTestSystemMetaData_h

class HGCalTestSystemMetaData
{
public:
  
  HGCalTestSystemMetaData(int trigType,int trigTime,int trigWidth)
    : trigType_(trigType), trigTime_(trigTime), trigWidth_(trigWidth) { }

  HGCalTestSystemMetaData() : HGCalTestSystemMetaData(0,0,0) { }

  
  ~HGCalTestSystemMetaData() {}

  uint32_t trigType_;
  uint32_t trigTime_;
  uint32_t trigWidth_;

};

#endif
