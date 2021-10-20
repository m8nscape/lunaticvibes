#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>

#include <map>
#include <utility>

#include "fmod.hpp"

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(QWidget *parent = nullptr);
    ~SetupDialog();

protected:
    std::map<std::string, size_t> mapProfile;
    std::string lr2path;

    enum class WindowMode
    {
        WINDOWED,
        FULL,
        BORDERLESS
    };

    enum class Resolution
    {
        SD_480p,
        HD_720p,
        FHD_1080p,
        WQHD_2k,
        QFHD_4k
    };

    FMOD::System* fmodsystem;

    std::vector<std::string> folders;

private:
    Ui::SetupDialog *ui;

    void refreshProfileList();
    bool loadConfig(const std::string& dirname);

public slots:
    void foldersCurrentRowChanged(int row);
    void addFolder();
    void delFolder();
    void openFolder();

    void saveConfig();

    void refreshAudioDeviceList();

    void runGame();
};

#endif // SETUPDIALOG_H
