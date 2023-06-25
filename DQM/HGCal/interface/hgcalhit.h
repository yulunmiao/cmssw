#ifndef __hgcalhit__
#define __hgcalhit__

#include <iostream>

struct FromPedData{};
struct FromRawData{};
class DetectorId{
public:
  DetectorId()
  {
    m_chip = 0;
    m_half = 0;
    m_channel = 0;
  }
  DetectorId(FromPedData, int chip, int channel, int channeltype) : m_chip(chip)
  {
    if( channeltype==0 ){ //i.e. channel goes between 0 ad 71
      m_half = channel/36;
      m_channel  = channel%36;
    }
    else if( channeltype==1 ){//i.e. channel = 0 or 1 for calib channels
      m_half = channel;
      m_channel  = 36;
    }
    else{//i.e. channel = 0, 1, 2 or 3 for CM channels
      m_half = channel/2;
      m_channel  = 37+channel%2;
    }    
  }
  DetectorId(FromRawData, int chip, int half, int channel) : m_chip(chip),
							    m_half(half),
							    m_channel(channel)
  {;}
							
  ~DetectorId(){;}
  friend bool operator==(const DetectorId& lhs, const DetectorId& rhs);
  inline int id() const {return m_chip*78+m_half*39+m_channel; }
  inline int chip() const {return m_chip;}
  inline int half() const {return m_half;}
  inline int channel() const {return m_channel;}
private:
  int m_chip;
  int m_half;
  int m_channel;
};

class Hit
{
public:
  Hit(){;}
  Hit(int event, DetectorId detid, int adc, int toa, int tot, int latency) : m_event(event),
									     m_detid(detid),
									     m_adc(adc),
									     m_toa(toa),
									     m_triglatency(latency)
  {;}
  ~Hit(){;}
  inline int event() const {return m_event;}
  inline DetectorId detid() const {return m_detid;}
  inline float adc() const {return m_adc;}
  inline int toa() const {return m_toa;}
  inline int tot() const {return m_tot;}
  inline int triglatency() const {return m_triglatency;}

  inline void set_adc(float adc){m_adc=adc;}

  friend std::ostream& operator<<(std::ostream& out,const Hit& rs);  
private:
  int m_event;
  DetectorId m_detid;
  float m_adc,m_toa,m_tot;
  int m_triglatency;
};

#endif
