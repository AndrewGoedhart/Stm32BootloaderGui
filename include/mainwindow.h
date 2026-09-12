#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <SerialSource.h>
#include <commands/commandSequence.h>
#include <softwareimage.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    static constexpr uint32_t PageSize = 256;
    static constexpr uint32_t FlashBootStart = 0x08000000;
    static constexpr uint32_t FlashAppStart = 0x08000800;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setupSerial();
    void enableButtons(bool enableDownload);

    /// @brief display a file dialog box to find and update the associated line edit.
    void loadFile(QString Prompt, QLineEdit *leFile);


    /// @brief Load a software image onto the device
    /// @details Allow for pre and post commands to be added to the load sequence.
    ///
    template <typename FN1, typename FN2> void download(QLineEdit *srcFile, uint32_t flashStart, FN1 preSequence, FN2 postSequence);


private slots:
    void on_tbLoadDc_clicked();
    void on_tbLoadBoot_clicked();
    void on_tbLoadMain_clicked();

    void on_btnBootloader_clicked();
    void on_btnDC_clicked();
    void on_btnApp_clicked();
    void on_btnReset_clicked();
    void on_btnDebug_clicked();

    void updateProgress(QString command, int percent);
    void downloadComplete();
    void downloadFailed();

private:
    void createDownloadSequence(CommandSequence *seq, SoftwareImage *image, uint32_t startAddress);

    Ui::MainWindow *ui;
    SerialSource *_serial;
    QString       _actionLabel;
    CommandSequence *_softwareCmds;

};
#endif // MAINWINDOW_H
