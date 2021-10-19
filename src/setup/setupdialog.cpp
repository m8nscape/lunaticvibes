#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <QTextStream>

#include "common/meta.h"
#include "config/config_mgr.h"

#include <filesystem>

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    fmodsystem = NULL;
    FMOD::System_Create(&fmodsystem);

    ui->setupUi(this);

    // set title
    QTextStream title;
    title << PROJECT_NAME << ' ' << "Setup";
    title.flush();
    setWindowTitle(*title.string());

    // set version
    ui->label_version->setText(QString::fromUtf8(PROJECT_VERSION));

    // get profile list
    refreshProfileList();

    // set profile
    std::string profile = ConfigMgr::get('E', cfg::E_PROFILE, PROFILE_DEFAULT);
    if (auto it = std::find(profileList.begin(), profileList.end(), profile); it != profileList.end())
    {
        loadProfile(profile);

        currentProfile = std::distance(profileList.begin(), it);
    }

    // set lr2 path
    lr2path = ConfigMgr::get('E', cfg::E_LR2PATH, ".");

    // folders
    auto folders = ConfigMgr::General()->getFolders();
}

SetupDialog::~SetupDialog()
{
    saveProfile();

    delete ui;

    fmodsystem->release();
}


void SetupDialog::refreshProfileList()
{

}

bool SetupDialog::loadProfile(size_t idx)
{
    if (!std::filesystem::is_directory("profile/"s + profileList[idx])) return false;

    ConfigMgr::selectProfile(profileList[idx]);

    /////////////////////////////////////////////////////////////
    // Video
    {
        // get resolution
        int y = ConfigMgr::get('V', cfg::V_RES_X, 720);
        switch (y)
        {
        case 640:  ui->resolution_list->setCurrentIndex((int)Resolution::SD_480p); break;
        case 720:  ui->resolution_list->setCurrentIndex((int)Resolution::HD_720p); break;
        case 1080: ui->resolution_list->setCurrentIndex((int)Resolution::FHD_1080p); break;
        default: break;
        }

        // get window mode
        auto wms = ConfigMgr::get('V', cfg::V_WINMODE, cfg::V_WINMODE_WINDOWED);
        if      (wms == cfg::V_WINMODE_WINDOWED)   ui->windowed->setChecked(true);
        else if (wms == cfg::V_WINMODE_FULL)       ui->fullscreen->setChecked(true);
        else if (wms == cfg::V_WINMODE_BORDERLESS) ui->borderless->setChecked(true);

        // get vsync
        ui->vsync->setChecked(!!ConfigMgr::get('V', cfg::V_VSYNC, 0));
    }


    /////////////////////////////////////////////////////////////
    // Audio
    {
        // set backend
        refreshAudioDeviceList();
        auto adev = ConfigMgr::get('A', cfg::A_DEVNAME, "");
        auto amod = ConfigMgr::get('A', cfg::A_MODE, "");
        for (auto& device: audioDeviceList)
        {
            if (adev == device.name && amod == device.mode)
            {
                //currentAudioDevice = std::distance(audioDeviceList.begin(), it);
            }
        }

        // set buffer count / size
        ui->audio_buffer_count->setText(QString::number(ConfigMgr::get('A', cfg::A_BUFCOUNT, 4)));
        ui->audio_buffer_size->setText(QString::number(ConfigMgr::get('A', cfg::A_BUFLEN, 128)));
    }


    /////////////////////////////////////////////////////////////
    // Folders
    {
        
    }


    /////////////////////////////////////////////////////////////
    // Advanced
    {

    }


    return true;
}


bool SetupDialog::saveProfile()
{
    // check if selecting a valid profile
    
    // fetch from widget, save

    ConfigMgr::save();
    return true;
}

void SetupDialog::refreshAudioDeviceList()
{
    audioDeviceList.clear();
    if (!fmodsystem) return;

    int numDrivers = 0;
    char name[512];
    FMOD_GUID guid;
    int systemrate;
    FMOD_SPEAKERMODE speakermode;
    int speakermodechannels;

    // Auto
    fmodsystem->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
    fmodsystem->getNumDrivers(&numDrivers);
    for (int i = 0; i < numDrivers; ++i)
    {
        if (FMOD_OK == fmodsystem->getDriverInfo(i, name, sizeof(name), &guid, &systemrate, &speakermode, &speakermodechannels))
        {
            audioDeviceList.push_back({ name, cfg::A_MODE_AUTO });
        }
    }

#ifdef WIN32
    // ASIO
    fmodsystem->setOutput(FMOD_OUTPUTTYPE_ASIO);
    fmodsystem->getNumDrivers(&numDrivers);
    for (int i = 0; i < numDrivers; ++i)
    {
        if (FMOD_OK == fmodsystem->getDriverInfo(i, name, sizeof(name), &guid, &systemrate, &speakermode, &speakermodechannels))
        {
            audioDeviceList.push_back({ name, cfg::A_MODE_ASIO });
        }
    }
#endif

}