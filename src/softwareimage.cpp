#ifndef SOFTWAREIMAGE_CPP
#define SOFTWAREIMAGE_CPP

#include <QFile>

#include <softwareimage.h>
#include <crcs/Crc32.hpp>


SoftwareImage::SoftwareImage(uint32_t pageSize):
  _pageSize(pageSize),
  _data(),
 _failureReason(""){
}

/**
 * LOad the data from the file returning an empty byte array if no file is found
 * @brief SoftwareImage::readFile
 * @param filename
 * @return
 */
bool SoftwareImage::openFile(const QString &filename){
  QFile file(filename);
  if( !file.exists()){
    _failureReason = "File does not exist";
    _data = {};
    return false;
  }

  if( file.open(QIODevice::ReadOnly)){
    auto temp = file.readAll();
    _data = {};
    _data.insert(_data.end(), temp.begin(), temp.end());
    file.close();
  }

  if( _data.size() ==0){
    _failureReason = "Invalid Image File";
    return false;
  }
  _failureReason = "";
  return true;
}


/**
 * @brief getDataSegment return the data for the segement given by the start address
 * and length
 * @param address the start of the data segment
 * @param length the requested length
 * @return a byte array contianing the request data. may contain less then the requested d
 */
Bytes SoftwareImage::getDataSegment(uint32_t address, uint32_t length){
  auto imageSize = getImageSize();
  if( address > imageSize ){
    return Bytes();
  }

  // adjust for image overruns
  if( address + length > imageSize){
    length = imageSize - address;
  }

  // adjust for page boundaries
  if( (address / _pageSize) != ((address+length)/_pageSize)){
    uint32_t endOfPage = ((address+length)/_pageSize)*_pageSize;
    length = endOfPage - address;
  }
  return Bytes(_data.begin() + address, _data.begin() + address+length);
}


QString SoftwareImage::getFailureReason(){
  return _failureReason;
}

uint32_t SoftwareImage::getImageSize(){
  return static_cast<uint32_t>(_data.size());
}

uint32_t SoftwareImage::getCrc(uint32_t size) {
  auto crc = Crcs::Crc<uint32_t, 0x4C11DB7U, 16U, 0xFFFFFFFFU, 0x00000000U >();
  auto imageSize = size/4;
  for( int i=0; i < imageSize; i++){
    uint32_t dword = _data[i*4] <<24 |
                     _data[i*4+1] << 16 |
                     _data[i*4+2] << 8 |
                     _data[i*4+3];
    crc.write32(dword);
  }
  return crc.getCrc();
}


#endif // SOFTWAREIMAGE_CPP
