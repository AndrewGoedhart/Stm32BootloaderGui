#ifndef SOFTWARE_DOWNLOAD_SOFTWARE_IMAGE_H
#define SOFTWARE_DOWNLOAD_SOFTWARE_IMAGE_H

#include <vector>
#include <QObject>
#include <QString>

using Bytes = std::vector<uint8_t>;
class SoftwareImage {
  public:
    /**
     */
    SoftwareImage(uint32_t pageSize);

    /**
       * @brief readFile read the whole file into a Byte array
       * @param file the file to read
       * @return the Byte array or an empty one if failed.
       */
    bool openFile(const QString &filename);

    /**
     * @brief getDataSegment return the data for the segement given by the start address 
     * and length
     * @param address the start of the data segment
     * @param length the requested length
     * @return a byte array contianing the request data. may contain less then the requested d
     */
   Bytes getDataSegment(uint32_t address, uint32_t length);

    /**
     * Get the reason for failure. if open returns false;
     */
    QString getFailureReason();

    uint32_t getImageSize();

    uint32_t getCrc(uint32_t size);

  private:
    uint32_t      _pageSize;
    Bytes         _data;
    QString       _failureReason;


};

#endif
