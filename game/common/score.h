#pragma once

class vScore
{
public:
    enum class Type
    {
        UNKNOWN,
        CLASSIC,
    };
    Type type = Type::UNKNOWN;

    int notes = 0;
    int score = 0;
    double rate = 0.0;
    int fast = 0;
    int slow = 0;
    int reserved[4]{ 0 };
    double reservedlf[2]{ 0.0 };

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
