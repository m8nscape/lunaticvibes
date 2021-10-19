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
    std::vector<std::string> profileList;

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
    struct audioBackend
    {
        std::string name;
        std::string mode;
    };
    std::vector<audioBackend> audioDeviceList;

    std::vector<std::string> folders;

private:
    Ui::SetupDialog *ui;

    void refreshProfileList();
    bool loadProfile(size_t idx);
    bool saveProfile();

    void refreshAudioDeviceList();
};

#endif // SETUPDIALOG_H
