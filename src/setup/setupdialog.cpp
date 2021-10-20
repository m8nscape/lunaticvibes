#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <QFileDialog>
#include <QListWidget>

#include "common/meta.h"
#include "config/config_mgr.h"

#include <filesystem>
#include <cstdlib>

SetupDialog::SetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    fmodsystem = NULL;
    FMOD::System_Create(&fmodsystem);

    ui->setupUi(this);

    // set title
    constexpr char title[] = PROJECT_NAME " Setup";
    setWindowTitle(QString::fromUtf8(title));

    // set version
    constexpr char version[] = PROJECT_SUB_NAME " " PROJECT_VERSION;
    ui->label_version->setText(QString::fromUtf8(version));


    // set profile
    std::string profile = ConfigMgr::get('E', cfg::E_PROFILE, cfg::PROFILE_DEFAULT);
    loadConfig(profile);

    // generate profile list
    refreshProfileList();
    bool hasProfile = false;
    for (int i = 0; i < ui->profile_list->count(); ++i)
    {
        if (strEqual(ui->profile_list->itemData(i).toString().toStdString(), profile, true))
        {
            ui->profile_list->setCurrentIndex(i);
            hasProfile = true;
        }
    }
    if (!hasProfile)
    {
        std::string name = profile + " [MISSING]";
        ui->profile_list->addItem(QString::fromStdString(name), QString::fromStdString(profile));
        ui->profile_list->setCurrentIndex(ui->profile_list->count() - 1);
    }

    QObject::connect(ui->browse_lr2path, &QPushButton::clicked, this, &SetupDialog::browseLR2Path);
    QObject::connect(ui->folder_list, &QListWidget::currentRowChanged, this, &SetupDialog::foldersCurrentRowChanged);
    QObject::connect(ui->add_folder, &QPushButton::clicked, this, &SetupDialog::addFolder);
    QObject::connect(ui->delete_folder, &QPushButton::clicked, this, &SetupDialog::delFolder);
    QObject::connect(ui->open_folder, &QPushButton::clicked, this, &SetupDialog::openFolder);
    QObject::connect(ui->refresh_audio_backend, &QPushButton::clicked, this, &SetupDialog::refreshAudioDeviceList);
    QObject::connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SetupDialog::saveConfig);
    QObject::connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SetupDialog::saveConfig);
    QObject::connect(ui->rungame, &QPushButton::clicked, this, &SetupDialog::runGame);
}

SetupDialog::~SetupDialog()
{
    delete ui;

    fmodsystem->release();
}


void SetupDialog::refreshProfileList()
{
    ui->profile_list->clear();
    mapProfile.clear();

    static const Path profilePath = Path(GAMEDATA_PATH) / "profile";
    if (!std::filesystem::exists(profilePath))
    {
        std::filesystem::create_directories(profilePath);
    }
    for (auto& p : std::filesystem::directory_iterator(profilePath))
    {
        if (p.is_directory())
        {
            auto name = p.path().filename().string();
            mapProfile[name] = ui->profile_list->count();
            ui->profile_list->addItem(QString::fromStdString(name), QString::fromStdString(name));
        }
    }
}

bool SetupDialog::loadConfig(const std::string& dirname)
{
    ConfigMgr::selectProfile(dirname);
    ConfigMgr::load();

    /////////////////////////////////////////////////////////////
    // Generic
    {
        // set lr2 path
        ui->lr2path->setText(QString::fromStdString(ConfigMgr::get('E', cfg::E_LR2PATH, ".")));


        // folders
        for (auto& f : ConfigMgr::General()->getFoldersStr())
        {
            ui->folder_list->addItem(QString::fromStdString(f));
        }
    }

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
        ui->vsync->setChecked(!!ConfigMgr::get('V', cfg::V_VSYNC, false));

        // max fps
        ui->maxfps->setText(QString::number(ConfigMgr::get('V', cfg::V_MAXFPS, 480)));
    }


    /////////////////////////////////////////////////////////////
    // Audio
    {
        // set backend
        refreshAudioDeviceList();
        auto adev = ConfigMgr::get('A', cfg::A_DEVNAME, "");
        auto amod = ConfigMgr::get('A', cfg::A_MODE, "");
        for (int i = 0; i < ui->audio_backend_list->count(); ++i)
        {
            QStringList& device = ui->audio_backend_list->itemData(i).toStringList();
            if (!device.isEmpty() && adev == device[0].toStdString() && amod == device[1].toStdString())
            {
                ui->audio_backend_list->setCurrentIndex(i);
            }
        }

        // set buffer count / size
        ui->audio_buffer_count->setText(QString::number(ConfigMgr::get('A', cfg::A_BUFCOUNT, 4)));
        ui->audio_buffer_size->setText(QString::number(ConfigMgr::get('A', cfg::A_BUFLEN, 128)));
    }


    /////////////////////////////////////////////////////////////
    // Advanced
    {
        ui->select_scroll_speed_1->setText(QString::number(ConfigMgr::get('P', cfg::P_LIST_SCROLL_TIME_INITIAL, 300)));
        ui->select_scroll_speed_2->setText(QString::number(ConfigMgr::get('P', cfg::P_LIST_SCROLL_TIME_HOLD, 150)));
        ui->miss_bga_time->setText(QString::number(ConfigMgr::get('P', cfg::P_MISSBGA_LENGTH, 500)));
        ui->min_input_interval->setText(QString::number(ConfigMgr::get('P', cfg::P_MIN_INPUT_INTERVAL, 16)));
        ui->new_song_duration->setText(QString::number(ConfigMgr::get('P', cfg::P_NEW_SONG_DURATION, 6)));
        ui->mouse_analog->setChecked(ConfigMgr::get('P', cfg::P_MOUSE_ANALOG, false));
    }


    return true;
}

void SetupDialog::saveConfig()
{
    // check if selecting a valid profile
    std::string profile = ui->profile_list->currentData().toString().toStdString();
    static const Path profilePath = Path(GAMEDATA_PATH) / "profile" / profile;
    if (!std::filesystem::exists(profilePath)) std::filesystem::create_directories(profilePath);
    if (!std::filesystem::is_directory(profilePath)) return;

    /////////////////////////////////////////////////////////////
    // Generic
    {
        // set lr2 path
        ConfigMgr::set('E', cfg::E_LR2PATH, ui->lr2path->text().toStdString());

        // folders
        std::vector<std::string> folders;
        for (int i = 0; i < ui->folder_list->count(); ++i)
        {
            folders.push_back(ui->folder_list->item(i)->text().toStdString());
        }
        ConfigMgr::General()->setFolders(folders);
    }

    /////////////////////////////////////////////////////////////
    // Folders
    {

        for (auto& f : ConfigMgr::General()->getFoldersStr())
        {
            ui->folder_list->addItem(QString::fromStdString(f));
        }
    }


    /////////////////////////////////////////////////////////////
    // Video
    {
        // get resolution
        switch ((Resolution)ui->resolution_list->currentIndex())
        {
        case Resolution::HD_720p:
            ConfigMgr::set('V', cfg::V_RES_X, 1280);
            ConfigMgr::set('V', cfg::V_RES_Y, 720);
            break;
        case Resolution::FHD_1080p:
            ConfigMgr::set('V', cfg::V_RES_X, 1920);
            ConfigMgr::set('V', cfg::V_RES_Y, 1080);
            break;
        case Resolution::SD_480p:
        default:
            ConfigMgr::set('V', cfg::V_RES_X, 640);
            ConfigMgr::set('V', cfg::V_RES_Y, 480);
            break;
        }

        // get window mode
        if (ui->fullscreen->isChecked())
            ConfigMgr::set('V', cfg::V_WINMODE, cfg::V_WINMODE_FULL);
        else if (ui->borderless->isChecked())
            ConfigMgr::set('V', cfg::V_WINMODE, cfg::V_WINMODE_BORDERLESS);
        else
            ConfigMgr::set('V', cfg::V_WINMODE, cfg::V_WINMODE_WINDOWED);

        // get vsync
        ConfigMgr::set('V', cfg::V_VSYNC, ui->vsync->isChecked());

        // max fps
        ConfigMgr::set('V', cfg::V_MAXFPS, ui->maxfps->text().toInt());
    }


    /////////////////////////////////////////////////////////////
    // Audio
    {
        // set backend
        auto device = ui->audio_backend_list->currentData().toStringList();
        if (!device.isEmpty())
        {
            ConfigMgr::set('A', cfg::A_DEVNAME, device[0].toStdString());
            ConfigMgr::set('A', cfg::A_MODE,    device[1].toStdString());
        }

        // set buffer count / size
        ConfigMgr::set('A', cfg::A_BUFCOUNT, ui->audio_buffer_count->text().toInt());
        ConfigMgr::set('A', cfg::A_BUFLEN,   ui->audio_buffer_size->text().toInt());
    }


    /////////////////////////////////////////////////////////////
    // Advanced
    {
        ConfigMgr::set('P', cfg::P_LIST_SCROLL_TIME_INITIAL, ui->select_scroll_speed_1->text().toInt());
        ConfigMgr::set('P', cfg::P_LIST_SCROLL_TIME_HOLD, ui->select_scroll_speed_2->text().toInt());
        ConfigMgr::set('P', cfg::P_MISSBGA_LENGTH, ui->miss_bga_time->text().toInt());
        ConfigMgr::set('P', cfg::P_MIN_INPUT_INTERVAL, ui->min_input_interval->text().toInt());
        ConfigMgr::set('P', cfg::P_NEW_SONG_DURATION, ui->new_song_duration->text().toInt());
        ConfigMgr::set('P', cfg::P_MOUSE_ANALOG, ui->mouse_analog->isChecked());
    }

    ConfigMgr::save();
}

/////////////////////////////////////////////////////////////
// Slots

void SetupDialog::browseLR2Path()
{
    QFileDialog fileDialog(this);
    fileDialog.setOption(QFileDialog::Option::ShowDirsOnly, true);
    fileDialog.setFileMode(QFileDialog::FileMode::Directory);
    fileDialog.setViewMode(QFileDialog::ViewMode::Detail);
    if (fileDialog.exec())
    {
        auto f = fileDialog.selectedFiles()[0];
        ui->lr2path->setText(f);
    }
}


void SetupDialog::foldersCurrentRowChanged(int row)
{
    if (row < 0)
    {
        ui->delete_folder->setEnabled(false);
        ui->open_folder->setEnabled(false);
    }
    else
    {
        ui->delete_folder->setEnabled(true);
        ui->open_folder->setEnabled(true);
    }
}


void SetupDialog::addFolder()
{
    QFileDialog fileDialog(this);
    fileDialog.setOption(QFileDialog::Option::ShowDirsOnly, true);
    fileDialog.setFileMode(QFileDialog::FileMode::Directory);
    fileDialog.setViewMode(QFileDialog::ViewMode::Detail);
    if (fileDialog.exec())
    {
        for (auto& f : fileDialog.selectedFiles())
        {
            ui->folder_list->addItem(f);
        }
    }
}


void SetupDialog::delFolder()
{
    int idx = ui->folder_list->currentRow();
    if (idx < 0) return;
    if (auto p = ui->folder_list->takeItem(idx); p != nullptr)
    {
        delete p;
    }
}


void SetupDialog::openFolder()
{
    int idx = ui->folder_list->currentRow();
    if (idx < 0) return;

    std::string pathstr = Path(ui->folder_list->item(idx)->text().toStdString()).string();

#ifdef WIN32
    ShellExecute(NULL, "open", pathstr.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#elif defined __linux__
    // linux has many WMs that may have to handle differently
#endif
}


void SetupDialog::refreshAudioDeviceList()
{
    ui->audio_backend_list->clear();
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
            ui->audio_backend_list->addItem(
                QString::fromStdString(name), QStringList({ QString::fromStdString(name), QString::fromUtf8(cfg::A_MODE_AUTO) }));
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
            std::string namefmt = "[ASIO] "s + name;
            ui->audio_backend_list->addItem(
                QString::fromStdString(namefmt), QStringList({ QString::fromStdString(name), QString::fromUtf8(cfg::A_MODE_ASIO) }));
        }
    }
#endif

}

void SetupDialog::runGame()
{
    saveConfig();
    done(QDialog::Accepted);

    // run game
#ifdef WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    GetStartupInfo(&si);
    si.cb = sizeof(si);
    CreateProcess(NULL, "game.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
#elif defined __linux__
    execl("game", NULL);
#endif
}