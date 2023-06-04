#ifndef hgcal_slinkfromraw_FileReader_h
#define hgcal_slinkfromraw_FileReader_h

#include <iostream>
#include <fstream>
#include <sstream>

//#include "FileContinuationCloseRecord.h"
//#include "FileContinuationOpenRecord.h"
//#include "FileNamer.h"
#include "RecordPrinter.h"

namespace hgcal_slinkfromraw {

  class FileReader {
  public:
    
  FileReader() : _directory("dat/") {
    }

    void setDirectory(const std::string &s) {
      _directory=s+"/";
    }


    bool open(const std::string &f) {
      _inputFile.open(f,std::ios::binary);
      return (_inputFile?true:false);
    }

    //bool read(uint64_t *d, unsigned n) {
    //bool read(RecordHeader *h) {
    bool read(Record *h) {
      _inputFile.read((char*)h,8);
      if(!_inputFile) return false;

      _inputFile.read((char*)(h+1),8*h->payloadLength());
      
      //if(h->identifier()==RecordHeader::FileContinuationEof) {
      if(h->state()==FsmState::Continuing) {
	RecordContinuing rc;
	rc.deepCopy(h);
	rc.print();
	
	std::cout << "FileReader::read() closing file "
		  << _fileName.c_str() << std::endl;
	
	_inputFile.close();
	
	_fileNumber++;
	_fileName=setRunFileName(_runNumber,_linkNumber,_fileNumber);
	
	std::cout << "FileReader::read() opening file "
		  << _fileName.c_str() << std::endl;
	
	_inputFile.open(_directory+_fileName.c_str(),std::ios::binary);
	
	_inputFile.read((char*)h,8);
	if(!_inputFile) return false;

	_inputFile.read((char*)(h+1),8*h->payloadLength());
	
	h->print();
	assert(h->state()==FsmState::Continuing);
	assert(((RecordContinuing*)h)->runNumber()     ==rc.runNumber());
	assert(((RecordContinuing*)h)->fileNumber()    ==rc.fileNumber()+1);
	assert(((RecordContinuing*)h)->numberOfEvents()==rc.numberOfEvents());

	_inputFile.read((char*)h,8);
	if(!_inputFile) return false;
	
	_inputFile.read((char*)(h+1),8*h->payloadLength());
      }
      
      return true;
    }
      
    bool close() {
      if(_inputFile.is_open()) {
	std::cout << "FileReader::close() closing file "
		  << _directory+_fileName << std::endl;
	_inputFile.close();
      }
      return true;
    }

    bool closed() {
      return !_inputFile.is_open();
    }

    static std::string setRelayFileName(uint32_t r) {
      std::ostringstream sRelayFileName;
      sRelayFileName << std::setfill('0')
		   << "Relay" << std::setw(10) << r
		   << ".bin";
      return sRelayFileName.str();
    }
    
    static std::string setRunFileName(uint32_t r, uint32_t l, uint32_t f) {
      std::ostringstream sRunFileName;
      sRunFileName << std::setfill('0')
		   << "Run" << std::setw(10) << r
		   << "_Link" << l
		   << "_File" << std::setw(10) << f
		   << ".bin";
      return sRunFileName.str();
    }
    
    protected:
    bool _relay;
    uint32_t _runNumber;
    uint32_t _linkNumber;
    uint32_t _fileNumber;
    
    std::string _directory;
    std::string _fileName;
    std::ifstream _inputFile;
  };
}

#endif
