#ifndef SERIAL_SOURCE
#define SERIAL_SOURCE

#include <memory>
#include <vector>
#include <queue>

#include <QQueue>
#include <QTimer>
#include <QObject>
#include <QByteArray>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

using Bytes = std::vector<uint8_t>;

class SerialSource : public QObject
{
    Q_OBJECT

 public:
    explicit SerialSource(QObject *parent = nullptr);
    virtual ~SerialSource();


    bool isOpen();
    QString getPort();
    uint32_t getBaudRate();

 public slots:
    void dtr(bool isEnabled);
    void rts(bool isEnabled);

    /**
     *
     */
    void txData(const Bytes &data);

    /**
     * Open the given serial port with the given baud rate
     * @brief openPort Open the serial port with 1 start an stop bit, 8 data bits and no flow control
     * @param portName the port to use
     * @param baudRate the baud rate to set the port to
     */
    void openPort(const QString &portName, int baudRate);

    /**
     * Close the serial port shutting down any transmissions in progress
     * @brief closePort
     */
    void closePort();

    /**
     * Clear any send or receive queues.
     * @brief clearQueues
     */
    void clearQueues();



  private slots:
    /**
     * @brief bytesWritten called by underlying serial port once the
     * previus message has been sent.
     * @param bytes the number of bytes sent.
     */
    void bytesWritten(qint64 bytes);

    void readyRead();
    /**
     * Called when an error occurs on the port.
     * @brief errorOccured
     * @param error
     */
    void errorOccured(QSerialPort::SerialPortError error);

    void poll();


 signals:
    /**
     * notify the channel that is waiting that we have received a
     * block of data
     */
    void rxData(const QByteArray &data);

    /**
     * @brief serialPortOpened The serial port opened successfully
     */
    void opened();

    /**
     * @brief the serial port was closed.
     */
    void closed();



  private:
    /**
     * Send the next packet of data in the queue if we have finished trransmitting the last one.
     * @brief sendIfTxComplete
     */
    void sendIfTxComplete();


    QSerialPort *_serialPort;
    std::unique_ptr<std::queue<Bytes>> _txQueue;
    bool _isTransmitting;
    QTimer *_retry;
};


#endif
