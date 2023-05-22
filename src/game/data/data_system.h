#pragma once

namespace lunaticvibes
{

class SongDB;
class ScoreDB;
extern std::shared_ptr<SongDB> g_pSongDB;
extern std::shared_ptr<ScoreDB> g_pScoreDB;

enum class GameWindowMode
{
    FULLSCREEN,
    BORDERLESS,
    WINDOWED
};
enum class GameVsyncMode {
    OFF,
    ON,
    ADAPTIVE
};

enum class FXType
{
    Off,
    Compressor,
    Echo,
    SfxReverb,
    LowPass,
    HighPass,

    // ...
};

enum class FreqModifierType
{
    Off,
    Frequency,
    PitchOnly,
    SpeedOnly,
};

inline struct Struct_SystemData
{
    std::unordered_map<std::string_view, long long> timers;

    bool isAppExiting = false;
    bool quitOnFinish = false;
    SceneType gNextScene = SceneType::SELECT;

    GameWindowMode windowMode = GameWindowMode::WINDOWED;
    GameVsyncMode vsyncMode = GameVsyncMode::OFF;
    unsigned currentRenderFPS = 0;
    unsigned currentInputFPS = 0;
    unsigned currentUpdateFPS = 0;

    Time sceneUpdateTime;

    unsigned dateYear = 0;
    unsigned dateMonthOfYear = 0;
    unsigned dateDayOfMonth = 0;
    unsigned timeHour = 0;
    unsigned timeMin = 0;
    unsigned timeSec = 0;

    double volumeMaster = 0.8;
    double volumeKey = 0.8;
    double volumeBgm = 0.8;

    bool equalizerEnabled = false;
    int equalizerVal62_5hz = 0;
    int equalizerVal160hz = 0;
    int equalizerVal400hz = 0;
    int equalizerVal1khz = 0;
    int equalizerVal2_5khz = 0;
    int equalizerVal6_25khz = 0;
    int equalizerVal16khz = 0;

    FXType fxType = FXType::Off;
    double fxVal = 0.0;

    FreqModifierType freqType = FreqModifierType::Off;
    int freqVal = 0;    // -12 ~ 12
    double pitchSpeed = 1.;

    bool IROnline = false;

    StringContent playerName;

    int scratchAxisValue[2] = { 0, 0 };

    StringContent overlayTopLeftText[4];

    class NotificationTextManager
    {
    public:
        static constexpr const size_t MAX_NOTIFICATIONS = 10;
        static constexpr const int EXPIRE_TIME_SEC = 10;
    private:
        std::shared_mutex mutex;
        std::deque<std::pair<Time, StringContent>> notifications;

    public:
        void newNotification(const StringContent& s)
        {
            std::unique_lock l(mutex);
            if (notifications.size() > MAX_NOTIFICATIONS)
                notifications.pop_front();
            notifications.push_back({ Time(), s});
        }
        void clearExpiredNotification()
        {
            std::unique_lock l(mutex);
            Time t;

            while (!notifications.empty() && (t - notifications.begin()->first).norm() > EXPIRE_TIME_SEC * 1000)
            {
                notifications.pop_front();
            }
        }
        StringContent getNotification(size_t index)
        {
            std::shared_lock l(mutex);
            return index < notifications.size() ? notifications[index].second : "";
        }
        std::shared_lock<std::shared_mutex> acquire()
        {
            return std::shared_lock(mutex);
        }
    } overlayTextManager;

    bool isEditingText = false;

} SystemData;

void setFreqModifier(FreqModifierType type, int val);

}