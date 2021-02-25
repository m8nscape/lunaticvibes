#pragma once

class vScore
{
public:
    enum class Type
    {
        UNKNOWN,
        CLASSIC,
    };
    Type type;

    int notes;
    int score;
    double rate;
    int fast;
    int slow;
    int reserved[4];
    double reservedlf[2];

public:
    vScore(Type t) : type(t) {}
};

class ScoreClassic : public vScore
{
public:
    ScoreClassic() : vScore(Type::CLASSIC) {}

public:
    int exscore;

    enum class Lamp
    {
        NOPLAY,
        FAILED,
        ASSIST,
        EASY,
        NORMAL,
        HARD,
        EXHARD,
        FULLCOMBO,
        PERFECT,
        MAX
    };
    Lamp lamp;

    int pgreat;
    int great;
    int good;
    int bad;
    int bpoor;
    int miss;
    int bp;
};
