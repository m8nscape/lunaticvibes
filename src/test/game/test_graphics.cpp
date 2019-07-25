#include "game/graphics/sprite.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class mock_Image : public Image
{
public:
    mock_Image() : Image("") {}
};

static Rect TEST_RECT{ 0, 0, 256, 256 };
static timestamp t0{ 1 }, t1{ 2 }, t2{ 3 }, t3{ 4 }, t4{ 5 }, t5{ 6 }, t6{ 7 }, t7{ 8 };
class mock_Texture : public Texture
{
public:
    mock_Texture() :Texture(mock_Image()) { _texRect = TEST_RECT; _loaded = true; }
    MOCK_CONST_METHOD6(draw, void(const Rect& srcRect, Rect dstRect,
        const Color c, const BlendMode blend, const bool filter, const double angleInDegrees));
};


TEST(Graphics_Color, construct)
{
    Color c{ 0x10, 0x20, 0x40, 0x80 };
    EXPECT_EQ(c, c);
    EXPECT_EQ(c, Color(c));
    EXPECT_EQ(c, Color(0x10204080));
    EXPECT_EQ(c.hex(), 0x10204080);

    Color d{ 0x10, 0x0, 0xFF, 0xFF };
    EXPECT_NE(c, d);
    EXPECT_EQ(d, Color(0x1000FFFF));
    EXPECT_EQ(d, Color(0x10, 0x0, 1234, 999));
    EXPECT_EQ(d, Color(0x10, -8124, 255, 255));
}

TEST(Graphics_Color, arithmetic)
{
    Color c{ 0x01020304 };
    Color d{ 0x05060708 };
    EXPECT_EQ(c + d, Color(0x06080A0C));
    EXPECT_EQ(c * 2, Color(0x02040608));
    EXPECT_EQ(c * -2, 0);
    //EXPECT_EQ(2 * c, d);
}


TEST(Graphics_Rect, self_equal)
{
    Rect r1{ 0, 0, 40, 60 };
    EXPECT_EQ(r1, r1);

    Rect r2{};
    EXPECT_EQ(r2, r2);

    Rect r3{ 20, 30 };
    EXPECT_EQ(r3, r3);

    Rect r4{ 10, 20, 30, 40 };
    Rect r4_c = r4;
    EXPECT_EQ(r4, r4_c);

    EXPECT_NE(r1, r2);
    EXPECT_NE(r1, r3);
    EXPECT_NE(r1, r4);
    EXPECT_NE(r2, r3);
    EXPECT_NE(r2, r4);
    EXPECT_NE(r3, r4);
}

TEST(Graphics_Rect, construct)
{
    EXPECT_EQ(Rect(0, 0, 40, 60), Rect(40, 60));
    EXPECT_EQ(Rect(-1, -1, -1, -1), Rect());
    EXPECT_EQ(Rect(-1, -1, -1, -1), Rect(0));
    EXPECT_EQ(Rect(-1, -1, -1, -1), Rect(200));

    Rect r{ 1, 2, 3, 4 };
    Rect r1(r);
    EXPECT_EQ(r, r1);
}

TEST(Graphics_Rect, wrapping)
{
    Rect r0{ 0, 0, 1024, 1024 };

    Rect full1{};
    EXPECT_EQ(r0, full1.standardize(r0));

    Rect full2{ -1, -1 };
    EXPECT_EQ(r0, full2.standardize(r0));

    Rect full3{ 0, 0, -1, -1 };
    EXPECT_EQ(r0, full3.standardize(r0));

    Rect r_oobx{ -300, 0, 1024, 1024 };
    EXPECT_NE(r0, r_oobx.standardize(r0));
    Rect r_ooby{ 0, -40, 1024, 1024 };
    EXPECT_NE(r0, r_ooby.standardize(r0));
    Rect r_oobw{ 0, 0, 2048, 1024 };
    EXPECT_NE(r0, r_oobw.standardize(r0));
    Rect r_oobh{ 0, 0, 1024, 2048 };
    EXPECT_NE(r0, r_oobh.standardize(r0));

    Rect r_oobx2{ 1025, 0, 1024, 1024 };
    EXPECT_NE(r0, r_oobx2.standardize(r0));
    Rect r_ooby2{ 0, 1025, 1024, 1024 };
    EXPECT_NE(r0, r_ooby2.standardize(r0));
    Rect r_oobw2{ 0, 0, 1025, 1024 };
    EXPECT_NE(r0, r_oobw2.standardize(r0));
    Rect r_oobh2{ 0, 0, 1024, 1025 };
    EXPECT_NE(r0, r_oobh2.standardize(r0));

    Rect large{ -1024, -1024, 4096, 4096 };
    EXPECT_NE(r0, large);

}

TEST(Graphics_Rect, add_normal)
{
    Rect r1{ 0, 0, 40, 60 };
    Rect r2{ 0, 0, 40, 60 };
    Rect res = r1 + r2;

}

////////////////////////////////////////////////////////////////////////////////
// Render interface

#pragma region Render Interface (vSprite)
class mock_vSprite : public vSprite
{
public:
    mock_vSprite(pTexture pTexture, SpriteTypes type = SpriteTypes::VIRTUAL) : vSprite(pTexture, type) {}
    MOCK_CONST_METHOD0(draw, void());
    FRIEND_TEST(test_Graphics_vSprite, rectConstruct);
    FRIEND_TEST(test_Graphics_vSprite, func_update);
};

class test_Graphics_vSprite : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    Rect rFull{ 0 };
    mock_vSprite ss1{ pt };
    mock_vSprite ss1_1{ pt };
    mock_vSprite ss1_2{ pt };
public:
    test_Graphics_vSprite()
    {
        ss1.setTimer(eTimer::K11_BOMB);
        ss1_1.setTimer(eTimer::K11_BOMB);
        ss1_2.setTimer(eTimer::K11_BOMB);

        ss1.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss1.setLoopTime(0);

        ss1_1.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss1_1.appendKeyFrame({ 255, {Rect(255, 255, 255, 255), RenderParams::CONSTANT, Color(0x00000000), BlendMode::ALPHA, 0, 0} });
        ss1_1.setLoopTime(-1);

        ss1_2.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss1_2.appendKeyFrame({ 255, {Rect(255, 255, 255, 255), RenderParams::CONSTANT, Color(0x00000000), BlendMode::ALPHA, 0, 0} });
        ss1_2.setLoopTime(0);
    }
};

TEST_F(test_Graphics_vSprite, func_update)
{
    gTimers.set(eTimer::K11_BOMB, 0);
    timestamp t(0), t1(128), t2(255), t3(256), t4(512);

    ss1.update(t);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1.getCurrentRenderParams().rect, Rect(0, 0, 0, 0));
    ASSERT_EQ(ss1.getCurrentRenderParams().color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().angle, 0);

    ss1.update(t1);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1.getCurrentRenderParams().rect, Rect(0, 0, 0, 0));
    ASSERT_EQ(ss1.getCurrentRenderParams().color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().angle, 0);
    ss1_1.update(t1);
    ASSERT_TRUE(ss1_1._draw);
    ASSERT_EQ(ss1_1.getCurrentRenderParams().rect, Rect(128, 128, 128, 128));
    ASSERT_EQ(ss1_1.getCurrentRenderParams().color, Color(127, 127, 127, 127));
    ASSERT_EQ(ss1_1.getCurrentRenderParams().angle, 0);

    ss1.update(t2);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1.getCurrentRenderParams().rect, Rect(0, 0, 0, 0));
    ASSERT_EQ(ss1.getCurrentRenderParams().color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().angle, 0);
    ss1_1.update(t2);
    ASSERT_TRUE(ss1_1._draw);
    ASSERT_EQ(ss1_1.getCurrentRenderParams().rect, Rect(255, 255, 255, 255));
    ASSERT_EQ(ss1_1.getCurrentRenderParams().color, Color(0, 0, 0, 0));
    ASSERT_EQ(ss1_1.getCurrentRenderParams().angle, 0);

    ss1.update(t3);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1.getCurrentRenderParams().rect, Rect(0, 0, 0, 0));
    ASSERT_EQ(ss1.getCurrentRenderParams().color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().angle, 0);
    ss1_1.update(t3);
    ASSERT_FALSE(ss1_1._draw);
    ss1_2.update(t3);
    ASSERT_TRUE(ss1_2._draw);
    ASSERT_EQ(ss1_2.getCurrentRenderParams().rect, Rect(1, 1, 1, 1));
    ASSERT_EQ(ss1_2.getCurrentRenderParams().color, Color(254, 254, 254, 254));
    ASSERT_EQ(ss1_2.getCurrentRenderParams().angle, 0);

    ss1_2.update(t4);
    ASSERT_TRUE(ss1_2._draw);
    ASSERT_EQ(ss1_2.getCurrentRenderParams().rect, Rect(2, 2, 2, 2));
    ASSERT_EQ(ss1_2.getCurrentRenderParams().color, Color(253, 253, 253, 253));
    ASSERT_EQ(ss1_2.getCurrentRenderParams().angle, 0);
}
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Texture-split sprite:

#pragma region Texture-split Sprite (SpriteSelection)
class mock_SpriteSelection : public SpriteSelection
{
public:
    mock_SpriteSelection(pTexture texture,
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false) : SpriteSelection(texture, rows, cols, verticalIndexing) {}
    FRIEND_TEST(test_Graphics_SpriteSelection, rectConstruct);
};

class test_Graphics_SpriteSelection : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteSelection s0{ pt, 1, 1, false };
    mock_SpriteSelection s{ pt, 2, 4, false };
    mock_SpriteSelection sv{ pt, 2, 4, true };
public:
    test_Graphics_SpriteSelection()
    {
        s0.setTimer(eTimer::K11_BOMB);
        s0.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s0.setLoopTime(0);
        s.setTimer(eTimer::K11_BOMB);
        s.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s.setLoopTime(0);
        sv.setTimer(eTimer::K11_BOMB);
        sv.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sv.setLoopTime(0);
    }
};

TEST_F(test_Graphics_SpriteSelection, rectConstruct)
{
    EXPECT_EQ(s0._segments, 1);
    EXPECT_EQ(s0._texRect[0], TEST_RECT);

    int w = TEST_RECT.w / 4;
    int h = TEST_RECT.h / 2;
    EXPECT_EQ(s._segments, 8);
    EXPECT_EQ(s._texRect[0], Rect(0 * w, 0 * h, w, h));
    EXPECT_EQ(s._texRect[1], Rect(1 * w, 0 * h, w, h));
    EXPECT_EQ(s._texRect[2], Rect(2 * w, 0 * h, w, h));
    EXPECT_EQ(s._texRect[3], Rect(3 * w, 0 * h, w, h));
    EXPECT_EQ(s._texRect[4], Rect(0 * w, 1 * h, w, h));
    EXPECT_EQ(s._texRect[5], Rect(1 * w, 1 * h, w, h));
    EXPECT_EQ(s._texRect[6], Rect(2 * w, 1 * h, w, h));
    EXPECT_EQ(s._texRect[7], Rect(3 * w, 1 * h, w, h));

    EXPECT_EQ(sv._segments, 8);
    EXPECT_EQ(sv._texRect[0], Rect(0 * w, 0 * h, w, h));
    EXPECT_EQ(sv._texRect[1], Rect(0 * w, 1 * h, w, h));
    EXPECT_EQ(sv._texRect[2], Rect(1 * w, 0 * h, w, h));
    EXPECT_EQ(sv._texRect[3], Rect(1 * w, 1 * h, w, h));
    EXPECT_EQ(sv._texRect[4], Rect(2 * w, 0 * h, w, h));
    EXPECT_EQ(sv._texRect[5], Rect(2 * w, 1 * h, w, h));
    EXPECT_EQ(sv._texRect[6], Rect(3 * w, 0 * h, w, h));
    EXPECT_EQ(sv._texRect[7], Rect(3 * w, 1 * h, w, h));
}
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Animated sprite:

#pragma region Animated Sprite (SpriteAnimated)

class mock_SpriteAnimated : public SpriteAnimated
{
public:
    mock_SpriteAnimated(pTexture texture,
        unsigned animFrames, unsigned frameTime, eTimer timer = eTimer::SCENE_START, 
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false) : SpriteAnimated(texture, animFrames, frameTime, timer, rows, cols, verticalIndexing) {}
    FRIEND_TEST(test_Graphics_SpriteAnimated, animRectConstruct);
    FRIEND_TEST(test_Graphics_SpriteAnimated, animUpdate);
};

class test_Graphics_SpriteAnimated : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteAnimated s{ pt, 8, 8, eTimer::K11_BOMB, 4, 2 };
    mock_SpriteAnimated ss{ pt, 8,8, eTimer::K11_BOMB, 8, 4, false };
public:
    test_Graphics_SpriteAnimated()
    {
        s.setTimer(eTimer::K11_BOMB);
        s.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s.setLoopTime(0);
        ss.setTimer(eTimer::K11_BOMB);
        ss.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss.setLoopTime(0);
    }
};


TEST_F(test_Graphics_SpriteAnimated, animRectConstruct)
{
    int w = TEST_RECT.w / 2;
    int h = TEST_RECT.h / 4;
    int ww = TEST_RECT.w / 4;
    int hh = TEST_RECT.h / 8;

    EXPECT_EQ(s._segments, 1 * 8);
    EXPECT_EQ(s._animFrames, 8);
    //EXPECT_EQ(s._aRect, Rect(0, 0, w, h));

    EXPECT_EQ(ss._segments, 4 * 8);
    EXPECT_EQ(ss._animFrames, 8);
    //EXPECT_EQ(ss._aRect, Rect(0, 0, w/2 , h/2));
    EXPECT_EQ(ss._texRect[0], Rect(0, 0, ww, hh));
}

TEST_F(test_Graphics_SpriteAnimated, animUpdate)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    int w = TEST_RECT.w / 2;
    int h = TEST_RECT.h / 4;
    int ww = w / 2;
    int hh = h / 2;

    using namespace ::testing;
    InSequence dummy;

    s.update(t0);
    EXPECT_CALL(*pt, draw(Rect(0 * w, 0 * h, w, h), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
    s.update(t1);
    EXPECT_CALL(*pt, draw(Rect(1 * w, 0 * h, w, h), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
    s.update(t2);
    EXPECT_CALL(*pt, draw(Rect(0 * w, 1 * h, w, h), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
    s.update(t3);
    EXPECT_CALL(*pt, draw(Rect(1 * w, 1 * h, w, h), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
    s.update(t4);
    EXPECT_CALL(*pt, draw(Rect(0 * w, 2 * h, w, h), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
    s.update(t5);
    EXPECT_CALL(*pt, draw(Rect(1 * w, 2 * h, w, h), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
    s.update(t6);
    EXPECT_CALL(*pt, draw(Rect(0 * w, 3 * h, w, h), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
    s.update(t7);
    EXPECT_CALL(*pt, draw(Rect(1 * w, 3 * h, w, h), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    ss.update(t0);
    EXPECT_CALL(*pt, draw(Rect(0 * ww, 0 * hh, ww, hh), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    ss.draw();
    ss.update(t1);
    EXPECT_CALL(*pt, draw(Rect(1 * ww, 0 * hh, ww, hh), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    ss.draw();
    ss.update(t2);
    EXPECT_CALL(*pt, draw(Rect(2 * ww, 0 * hh, ww, hh), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    ss.draw();
    ss.update(t3);
    EXPECT_CALL(*pt, draw(Rect(3 * ww, 0 * hh, ww, hh), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    ss.draw();
    ss.update(t4);
    EXPECT_CALL(*pt, draw(Rect(0 * ww, 1 * hh, ww, hh), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    ss.draw();
    ss.update(t5);
    EXPECT_CALL(*pt, draw(Rect(1 * ww, 1 * hh, ww, hh), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    ss.draw();
    ss.update(t6);
    EXPECT_CALL(*pt, draw(Rect(2 * ww, 1 * hh, ww, hh), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    ss.draw();
    ss.update(t7);
    EXPECT_CALL(*pt, draw(Rect(3 * ww, 1 * hh, ww, hh), Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    ss.draw();

}
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Number sprite:

#pragma region Animated number Sprite (SpriteNumber)

class mock_SpriteNumber : public SpriteNumber
{
public:
    mock_SpriteNumber(pTexture texture, const Rect& rect, NumberAlign align, unsigned maxDigits,
        unsigned numRows, unsigned numCols, unsigned frameTime, eNumber num = eNumber::_TEST1, eTimer animtimer = eTimer::K11_BOMB,
        unsigned animFrames = 1, bool numVerticalIndexing = false):
        SpriteNumber(texture, rect, align, maxDigits, numRows, numCols, frameTime, num, animtimer, animFrames, numVerticalIndexing) {}

    FRIEND_TEST(test_Graphics_SpriteNumber, construct);
    FRIEND_TEST(test_Graphics_SpriteNumber, numberUpdate);
    FRIEND_TEST(test_Graphics_SpriteNumber, animUpdate);
};

class test_Graphics_SpriteNumber : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    Rect dstRect{ 0,0,10,10 };
    Color dstColor{ 0xFFFFFFFF };
    mock_SpriteNumber s1{ pt, Rect(0, 0, 100, 160), NumberAlign::NUM_ALIGN_LEFT, 1, 2, 5, 0 };
    mock_SpriteNumber s{ pt, Rect(0, 0, 100, 160), NumberAlign::NUM_ALIGN_LEFT, 4, 2, 5, 0 };
    mock_SpriteNumber sa{ pt, Rect(0, 0, 100, 160), NumberAlign::NUM_ALIGN_LEFT, 4, 4, 5, 2, eNumber::_TEST1, eTimer::K11_BOMB, 2, false };
    mock_SpriteNumber s11{ pt, Rect(0, 0, 110, 160), NumberAlign::NUM_ALIGN_LEFT, 4, 1, 11, 0 };
    mock_SpriteNumber sa11{ pt, Rect(0, 0, 110, 160), NumberAlign::NUM_ALIGN_LEFT, 4, 4, 11, 4, eNumber::_TEST1, eTimer::K11_BOMB, 4, false };
    mock_SpriteNumber s24{ pt, Rect(0, 0, 240, 160), NumberAlign::NUM_ALIGN_LEFT, 4, 2, 12, 0 };
    mock_SpriteNumber sa24{ pt, Rect(0, 0, 240, 160), NumberAlign::NUM_ALIGN_LEFT, 4, 8, 12, 4, eNumber::_TEST1, eTimer::K11_BOMB, 4, false };
    mock_SpriteNumber sr{ pt, Rect(0, 0, 110, 160), NumberAlign::NUM_ALIGN_RIGHT, 4, 1, 11, 0 };
    mock_SpriteNumber sc{ pt, Rect(0, 0, 110, 160), NumberAlign::NUM_ALIGN_CENTER, 4, 1, 11, 0 };
public:
    test_Graphics_SpriteNumber()
    {
        s1.setTimer(eTimer::K11_BOMB);
        s1.appendKeyFrame({ 0, {dstRect, RenderParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        s1.setLoopTime(0);
        s.setTimer(eTimer::K11_BOMB);
        s.appendKeyFrame({ 0, {dstRect, RenderParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        s.setLoopTime(0);
        sa.setTimer(eTimer::K11_BOMB);
        sa.appendKeyFrame({ 0, {dstRect, RenderParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        sa.setLoopTime(0);

        s11.setTimer(eTimer::K11_BOMB);
        s11.appendKeyFrame({ 0, {dstRect, RenderParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        s11.setLoopTime(0);
        sa11.setTimer(eTimer::K11_BOMB);
        sa11.appendKeyFrame({ 0, {dstRect, RenderParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        sa11.setLoopTime(0);
        s24.setTimer(eTimer::K11_BOMB);
        s24.appendKeyFrame({ 0, {dstRect, RenderParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        s24.setLoopTime(0);
        sa24.setTimer(eTimer::K11_BOMB);
        sa24.appendKeyFrame({ 0, {dstRect, RenderParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        sa24.setLoopTime(0);

        sr.setTimer(eTimer::K11_BOMB);
        sr.appendKeyFrame({ 0, {dstRect, RenderParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        sr.setLoopTime(0);
        sc.setTimer(eTimer::K11_BOMB);
        sc.appendKeyFrame({ 0, {dstRect, RenderParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        sc.setLoopTime(0);
    }
};

TEST_F(test_Graphics_SpriteNumber, construct)
{
    EXPECT_EQ(s1._sDigit.size(), 1);
    EXPECT_EQ(s1._numType, NumberType::NUM_TYPE_NORMAL);
    EXPECT_EQ(s._sDigit.size(), 4);
    EXPECT_EQ(s._numType, NumberType::NUM_TYPE_NORMAL);
    EXPECT_EQ(sa11._sDigit.size(), 4);
    EXPECT_EQ(sa11._numType, NumberType::NUM_TYPE_BLANKZERO);
    EXPECT_EQ(sa24._sDigit.size(), 4);
    EXPECT_EQ(sa24._numType, NumberType::NUM_TYPE_FULL);
}

TEST_F(test_Graphics_SpriteNumber, numberUpdate)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    gNumbers.set(eNumber::_TEST1, 1);
    s.update(t0);
    EXPECT_THAT(s._digit, ElementsAre(1, 0, 0, 0));
    sa11.update(t0);
    EXPECT_EQ(sa11._digit[0], 1);
    sa24.update(t0);
    EXPECT_EQ(sa24._digit[0], 1);
    EXPECT_EQ(sa24._digit[1], NUM_FULL_PLUS);

    gNumbers.set(eNumber::_TEST1, 23);
    s.update(t0);
    EXPECT_THAT(s._digit, ElementsAre(3, 2, 0, 0));
    sa11.update(t0);
    EXPECT_EQ(sa11._digit[0], 3);
    EXPECT_EQ(sa11._digit[1], 2);
    sa24.update(t0);
    EXPECT_EQ(sa24._digit[0], 3);
    EXPECT_EQ(sa24._digit[1], 2);
    EXPECT_EQ(sa24._digit[2], NUM_FULL_PLUS);

    gNumbers.set(eNumber::_TEST1, 456);
    s.update(t0);
    EXPECT_THAT(s._digit, ElementsAre(6, 5, 4, 0));
    sa11.update(t0);
    EXPECT_EQ(sa11._digit[0], 6);
    EXPECT_EQ(sa11._digit[1], 5);
    EXPECT_EQ(sa11._digit[2], 4);
    sa24.update(t0);
    EXPECT_THAT(sa24._digit, ElementsAre(6, 5, 4, NUM_FULL_PLUS));

    gNumbers.set(eNumber::_TEST1, 1234);
    s.update(t0);
    EXPECT_THAT(s._digit, ElementsAre(4, 3, 2, 1));
    sa11.update(t0);
    EXPECT_THAT(sa11._digit, ElementsAre(4, 3, 2, 1));
    sa24.update(t0);
    EXPECT_THAT(sa24._digit, ElementsAre(4, 3, 2, NUM_FULL_PLUS));

    gNumbers.set(eNumber::_TEST1, 0);
    s.update(t0);
    EXPECT_THAT(s._digit, ElementsAre(0, 0, 0, 0));
    sa11.update(t0);
    EXPECT_EQ(sa11._digit[0], 0);
    sa24.update(t0);
    EXPECT_EQ(sa24._digit[0], 0);
    EXPECT_EQ(sa24._digit[1], NUM_FULL_PLUS);

    gNumbers.set(eNumber::_TEST1, 9999);
    s.update(t0);
    EXPECT_THAT(s._digit, ElementsAre(9, 9, 9, 9));

    gNumbers.set(eNumber::_TEST1, 2147483647);
    s.update(t0);
    EXPECT_THAT(s._digit, ElementsAre(7, 4, 6, 3));

    gNumbers.set(eNumber::_TEST1, -65532);
    s.update(t0);
    EXPECT_THAT(s._digit, ElementsAre(2, 3, 5, 5));
    sa11.update(t0);
    EXPECT_THAT(sa11._digit, ElementsAre(2, 3, 5, 5));
    sa24.update(t0);
    EXPECT_THAT(sa24._digit, ElementsAre(12+2, 12+3, 12+5, NUM_FULL_MINUS));

    gNumbers.set(eNumber::_TEST1, 0);
    s.update(t0);
    EXPECT_THAT(s._digit, ElementsAre(0, 0, 0, 0));
    sa11.update(t0);
    EXPECT_EQ(sa11._digit[0], 0);
    sa24.update(t0);
    EXPECT_EQ(sa24._digit[0], 0);
    EXPECT_EQ(sa24._digit[1], NUM_FULL_PLUS);

}

TEST_F(test_Graphics_SpriteNumber, rectUpdate_normal_1)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());

    gNumbers.set(eNumber::_TEST1, 1);
    s1.update(t0);

    EXPECT_CALL(*pt, draw(Rect(20, 0, 20, 80), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
    s1.draw();

}


TEST_F(test_Graphics_SpriteNumber, rectUpdate_normal_4)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 456);
        s.update(t0);

        EXPECT_CALL(*pt, draw(Rect(80, 0, 20, 80), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(0, 80, 20, 80), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 80, 20, 80), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        s.draw();
    }
    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 1234);
        s.update(t0);

        EXPECT_CALL(*pt, draw(Rect(20, 0, 20, 80), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(40, 0, 20, 80), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(60, 0, 20, 80), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 0, 20, 80), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        s.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 6789);
        s.update(t0);

        EXPECT_CALL(*pt, draw(Rect(20, 80, 20, 80), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(40, 80, 20, 80), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(60, 80, 20, 80), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 80, 20, 80), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        s.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 0);
        s.update(t0);

        EXPECT_CALL(*pt, draw(Rect(0, 0, 20, 80), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        s.draw();
    }
}


TEST_F(test_Graphics_SpriteNumber, rectUpdate_bzero_4)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 123);
        s11.update(t0);

        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 160), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 160), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 160), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        // extra 0s for left align is not drawn
        //EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        s11.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 6789);
        s11.update(t0);

        EXPECT_CALL(*pt, draw(Rect(60, 0, 10, 160), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(70, 0, 10, 160), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 0, 10, 160), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(90, 0, 10, 160), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        s11.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 0);
        s11.update(t0);

        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 160), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        // extra 0s for left align is not drawn
        //EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        //EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        //EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        s11.draw();
    }
}



TEST_F(test_Graphics_SpriteNumber, rectUpdate_full_4)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 123);
        s24.update(t0);

        EXPECT_CALL(*pt, draw(Rect(220, 0, 20, 80), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // +
        EXPECT_CALL(*pt, draw(Rect(20, 0, 20, 80), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 1
        EXPECT_CALL(*pt, draw(Rect(40, 0, 20, 80), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 2
        EXPECT_CALL(*pt, draw(Rect(60, 0, 20, 80), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 3
        s24.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 6789);
        s24.update(t0);

        EXPECT_CALL(*pt, draw(Rect(220, 0, 20, 80), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  // +
        EXPECT_CALL(*pt, draw(Rect(140, 0, 20, 80), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 7
        EXPECT_CALL(*pt, draw(Rect(160, 0, 20, 80), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 8
        EXPECT_CALL(*pt, draw(Rect(180, 0, 20, 80), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 9
        s24.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, -20);
        s24.update(t0);

        EXPECT_CALL(*pt, draw(Rect(220, 80, 20, 80), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);   // -
        EXPECT_CALL(*pt, draw(Rect(40, 80, 20, 80), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);   // 2
        EXPECT_CALL(*pt, draw(Rect(0, 80, 20, 80), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);    // 0
        //EXPECT_CALL(*pt, draw(Rect(200, 80, 20, 80), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  // b
        s24.draw();
    }
}


TEST_F(test_Graphics_SpriteNumber, rectUpdate_bzero_4_right)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 123);
        sr.update(t0);

        EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 160), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 160), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 160), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sr.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 6789);
        sr.update(t0);

        EXPECT_CALL(*pt, draw(Rect(60, 0, 10, 160), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(70, 0, 10, 160), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 0, 10, 160), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(90, 0, 10, 160), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sr.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 0);
        sr.update(t0);

        EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 160), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sr.draw();
    }
}

TEST_F(test_Graphics_SpriteNumber, rectUpdate_bzero_4_anim)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    // time: 1
    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 123);
        sa11.update(t1);

        EXPECT_CALL(*pt, draw(Rect(10, 40, 10, 40), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 40, 10, 40), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(30, 40, 10, 40), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sa11.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 6789);
        sa11.update(t1);

        EXPECT_CALL(*pt, draw(Rect(60, 40, 10, 40), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(70, 40, 10, 40), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 40, 10, 40), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(90, 40, 10, 40), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sa11.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 0);
        sa11.update(t1);

        EXPECT_CALL(*pt, draw(Rect(0, 40, 10, 40), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sa11.draw();
    }

    //time: 3
    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 123);
        sa11.update(t3);

        EXPECT_CALL(*pt, draw(Rect(10, 120, 10, 40), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 120, 10, 40), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(30, 120, 10, 40), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sa11.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 6789);
        sa11.update(t3);

        EXPECT_CALL(*pt, draw(Rect(60, 120, 10, 40), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(70, 120, 10, 40), Rect(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 120, 10, 40), Rect(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(90, 120, 10, 40), Rect(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sa11.draw();
    }

    {
        InSequence dummy;
        gNumbers.set(eNumber::_TEST1, 0);
        sa11.update(t3);

        EXPECT_CALL(*pt, draw(Rect(0, 120, 10, 40), Rect(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sa11.draw();
    }
}
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Slider sprite:

#pragma region Slider Sprite (SpriteSlider)
class mock_SpriteSlider : public SpriteSlider
{
public:
    mock_SpriteSlider(pTexture texture, const Rect& rect, SliderDirection dir, int range,
        unsigned animFrames, unsigned frameTime, eSlider ind = eSlider::_TEST1, eTimer animtimer = eTimer::K11_BOMB,
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false) :
        SpriteSlider(texture, rect, dir, range, animFrames, frameTime, ind, animtimer, selRows, selCols, selVerticalIndexing) {}
};

class test_Graphics_SpriteSlider : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteSlider sL{ pt, TEST_RECT, SliderDirection::LEFT, 101, 1, 0 };
    mock_SpriteSlider sR{ pt, TEST_RECT, SliderDirection::RIGHT, 101, 1, 0};
    mock_SpriteSlider sU{ pt, TEST_RECT, SliderDirection::UP, 201, 1, 0 };
    mock_SpriteSlider sD{ pt, TEST_RECT, SliderDirection::DOWN, 201, 1, 0 };
public:
    test_Graphics_SpriteSlider()
    {
        sL.setTimer(eTimer::K11_BOMB);
        sL.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sL.setLoopTime(0);
        sR.setTimer(eTimer::K11_BOMB);
        sR.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sR.setLoopTime(0);
        sU.setTimer(eTimer::K11_BOMB);
        sU.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sU.setLoopTime(0);
        sD.setTimer(eTimer::K11_BOMB);
        sD.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sD.setLoopTime(0);
    }
};

TEST_F(test_Graphics_SpriteSlider, updateLeft)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sL;

    InSequence dummy;
    gSliders.set(eSlider::_TEST1, 0);
    s.update(t0);
    //EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    //s.draw();
    EXPECT_EQ(s.getCurrentRenderParams().rect, Rect(0, 0, 0, 0));

    gSliders.set(eSlider::_TEST1, 0.33);
    s.update(t0);
    //EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0 - 33, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    //s.draw();
    EXPECT_EQ(s.getCurrentRenderParams().rect, Rect(-33, 0, 0, 0));

    gSliders.set(eSlider::_TEST1, 0.50);
    s.update(t0);
    //EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0 - 50, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    //s.draw();
    EXPECT_EQ(s.getCurrentRenderParams().rect, Rect(-50, 0, 0, 0));

    gSliders.set(eSlider::_TEST1, 1.00);
    s.update(t0);
    //EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0 - 100, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    //s.draw();
    EXPECT_EQ(s.getCurrentRenderParams().rect, Rect(-100, 0, 0, 0));
}

TEST_F(test_Graphics_SpriteSlider, updateRight)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sR;

    InSequence dummy;
    gSliders.set(eSlider::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gSliders.set(eSlider::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0 + 33, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gSliders.set(eSlider::_TEST1, 0.5);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0 + 50, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gSliders.set(eSlider::_TEST1, 1);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0 + 100, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
}

TEST_F(test_Graphics_SpriteSlider, updateUp)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sU;

    InSequence dummy;
    gSliders.set(eSlider::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gSliders.set(eSlider::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0 - 66, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gSliders.set(eSlider::_TEST1, 0.50);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0 - 100, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gSliders.set(eSlider::_TEST1, 1.00);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0 - 200, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
}

TEST_F(test_Graphics_SpriteSlider, updateDown)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sD;

    InSequence dummy;
    gSliders.set(eSlider::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gSliders.set(eSlider::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0 + 66, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gSliders.set(eSlider::_TEST1, 0.505);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0 + 101, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gSliders.set(eSlider::_TEST1, 1.00);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0 + 200, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
}
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Bargraph sprite:

#pragma region Bargraph Sprite (SpriteBargraph)
class mock_SpriteBargraph : public SpriteBargraph
{
public:
    mock_SpriteBargraph(pTexture texture, const Rect& rect, BargraphDirection dir,
        unsigned animFrames, unsigned frameTime, eBargraph ind = eBargraph::_TEST1, eTimer animtimer = eTimer::K11_BOMB,
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false) :
        SpriteBargraph(texture, rect, dir, animFrames, frameTime, ind, animtimer, selRows, selCols, selVerticalIndexing) {}
};

class test_Graphics_SpriteBargraph : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteBargraph sL{ pt, TEST_RECT, BargraphDirection::LEFT, 1, 0 };
    mock_SpriteBargraph sR{ pt, TEST_RECT, BargraphDirection::RIGHT, 1, 0 };
    mock_SpriteBargraph sU{ pt, TEST_RECT, BargraphDirection::UP, 1, 0 };
    mock_SpriteBargraph sD{ pt, TEST_RECT, BargraphDirection::DOWN, 1, 0 };
public:
    test_Graphics_SpriteBargraph()
    {
        sL.setTimer(eTimer::K11_BOMB);
        sL.appendKeyFrame({ 0, {Rect(0, 0, 200, 200), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sL.setLoopTime(0);
        sR.setTimer(eTimer::K11_BOMB);
        sR.appendKeyFrame({ 0, {Rect(0, 0, 200, 200), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sR.setLoopTime(0);
        sU.setTimer(eTimer::K11_BOMB);
        sU.appendKeyFrame({ 0, {Rect(0, 0, 200, 200), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sU.setLoopTime(0);
        sD.setTimer(eTimer::K11_BOMB);
        sD.appendKeyFrame({ 0, {Rect(0, 0, 200, 200), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sD.setLoopTime(0);
    }
};

TEST_F(test_Graphics_SpriteBargraph, updateLeft)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sL;

    InSequence dummy;
    gBargraphs.set(eBargraph::_TEST1, 0);
    s.update(t0);
    //EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    //s.draw();
    EXPECT_EQ(s.getCurrentRenderParams().rect, Rect(200, 0, 0, 200));

    gBargraphs.set(eBargraph::_TEST1, 0.33);
    s.update(t0);
    //EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0 - 33, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    //s.draw();
    EXPECT_EQ(s.getCurrentRenderParams().rect, Rect(134, 0, 66, 200));

    gBargraphs.set(eBargraph::_TEST1, 0.50);
    s.update(t0);
    //EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0 - 50, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    //s.draw();
    EXPECT_EQ(s.getCurrentRenderParams().rect, Rect(100, 0, 100, 200));

    gBargraphs.set(eBargraph::_TEST1, 1.00);
    s.update(t0);
    //EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0 - 100, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    //s.draw();
    EXPECT_EQ(s.getCurrentRenderParams().rect, Rect(0, 0, 200, 200));
}

TEST_F(test_Graphics_SpriteBargraph, updateRight)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sR;

    InSequence dummy;
    gBargraphs.set(eBargraph::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 0, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gBargraphs.set(eBargraph::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 66, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gBargraphs.set(eBargraph::_TEST1, 0.5);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 100, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gBargraphs.set(eBargraph::_TEST1, 1);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 200, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
}

TEST_F(test_Graphics_SpriteBargraph, updateUp)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sU;

    InSequence dummy;
    gBargraphs.set(eBargraph::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 200, 200, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gBargraphs.set(eBargraph::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 134, 200, 66), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gBargraphs.set(eBargraph::_TEST1, 0.50);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 100, 200, 100), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gBargraphs.set(eBargraph::_TEST1, 1.00);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 200, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
}

TEST_F(test_Graphics_SpriteBargraph, updateDown)
{
    gTimers.set(eTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sD;

    InSequence dummy;
    gBargraphs.set(eBargraph::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 200, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gBargraphs.set(eBargraph::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 200, 66), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gBargraphs.set(eBargraph::_TEST1, 0.505);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 200, 101), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();

    gBargraphs.set(eBargraph::_TEST1, 1.00);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, Rect(0, 0, 200, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1);
    s.draw();
}
#pragma endregion


////////////////////////////////////////////////////////////////////////////////
// Option sprite:

#pragma region Option Sprite (SpriteOption)

class mock_SpriteOption : public SpriteOption
{
public:
    mock_SpriteOption(pTexture texture,
        unsigned animFrames, unsigned frameTime, eTimer timer = eTimer::K11_BOMB,
        unsigned rows = 1, unsigned cols = 1, bool verticalIndexing = false) : 
        SpriteOption(texture, animFrames, frameTime, timer, rows, cols, verticalIndexing) {}
    FRIEND_TEST(test_Graphics_SpriteOption, switchTest);
    FRIEND_TEST(test_Graphics_SpriteOption, optionTest);
};

class test_Graphics_SpriteOption : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteOption ss{ pt, 1, 1, eTimer::K11_BOMB, 1, 2 };
    mock_SpriteOption ss0{ pt, 1, 1, eTimer::K11_BOMB, 1, 1 };
    mock_SpriteOption so{ pt, 1, 1, eTimer::K11_BOMB, 2, 3 };
public:
    test_Graphics_SpriteOption()
    {
        ss.setTimer(eTimer::K11_BOMB);
        ss.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss.setLoopTime(0);
        ss.setInd(SpriteOption::opType::SWITCH, (unsigned)eSwitch::_TEST1);
        ss0.setTimer(eTimer::K11_BOMB);
        ss0.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss0.setLoopTime(0);
        ss0.setInd(SpriteOption::opType::SWITCH, (unsigned)eSwitch::_TEST1);
        so.setTimer(eTimer::K11_BOMB);
        so.appendKeyFrame({ 0, {Rect(0, 0, 0, 0), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        so.setLoopTime(0);
        so.setInd(SpriteOption::opType::OPTION, (unsigned)eOption::_TEST1);
    }
};

TEST_F(test_Graphics_SpriteOption, switchTest)
{
    ASSERT_EQ(ss._opType, SpriteOption::opType::SWITCH);
    ASSERT_EQ(ss0._opType, SpriteOption::opType::SWITCH);

    gTimers.set(eTimer::K11_BOMB, t0.norm());
    gSwitches.set(eSwitch::_TEST1, false);
    ss.update(t0);
    EXPECT_EQ(ss._segmentIdx, 0);
    gSwitches.set(eSwitch::_TEST1, true);
    ss.update(t0);
    EXPECT_EQ(ss._segmentIdx, 1);
    ss0.update(t0);
    EXPECT_EQ(ss0._segmentIdx, 0);
}


TEST_F(test_Graphics_SpriteOption, optionTest)
{
    ASSERT_EQ(so._opType, SpriteOption::opType::OPTION);
    gOptions.set(eOption::_TEST1, 0);
    so.update(t0);
    EXPECT_EQ(so._segmentIdx, 0);
    gOptions.set(eOption::_TEST1, 1);
    so.update(t0);
    EXPECT_EQ(so._segmentIdx, 1);
    gOptions.set(eOption::_TEST1, 5);
    so.update(t0);
    EXPECT_EQ(so._segmentIdx, 5);
    gOptions.set(eOption::_TEST1, 12);
    so.update(t0);
    EXPECT_EQ(so._segmentIdx, 5);

}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// GaugeGrid sprite:

#pragma region Grid type gauge Sprite (SpriteGaugeGrid)
class mock_SpriteGaugeGrid : public SpriteGaugeGrid
{
public:
    mock_SpriteGaugeGrid(pTexture texture, const Rect& rect,
        unsigned animFrames, unsigned frameTime, int dx, int dy, 
        unsigned min = 0, unsigned max = 100, unsigned grids = 50,
        eTimer timer = eTimer::K11_BOMB, eNumber num = eNumber::PLAY_1P_GROOVEGAUGE,
        unsigned selRows = 1, unsigned selCols = 1, bool selVerticalIndexing = false):
        SpriteGaugeGrid(texture, rect, animFrames, frameTime, dx, dy, min, max, grids, timer, num, selRows, selCols, selVerticalIndexing) {}
};

class test_Graphics_SpriteGaugeGrid : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteGaugeGrid s1{ pt, Rect(0, 0, 40, 40), 1, 0, 10, 0, 0, 100, 50, eTimer::K11_BOMB, eNumber::PLAY_1P_GROOVEGAUGE, 1, 4 };
    mock_SpriteGaugeGrid s2{ pt, Rect(0, 0, 60, 40), 1, 0, -10, 0, 0, 100, 50, eTimer::K11_BOMB, eNumber::PLAY_2P_GROOVEGAUGE, 1, 6 };
public:
    test_Graphics_SpriteGaugeGrid()
    {
        s1.setTimer(eTimer::K11_BOMB);
        s1.appendKeyFrame({ 0, {Rect(0, 0, 10, 40), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s1.setLoopTime(0);
        s1.setFlashType(SpriteGaugeGrid::FlashType::NONE);
        s1.setGaugeType(SpriteGaugeGrid::GaugeType::NORMAL);
        s2.setTimer(eTimer::K11_BOMB);
        s2.appendKeyFrame({ 0, {Rect(490, 100, 10, 40), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s2.setLoopTime(0);
        s2.setFlashType(SpriteGaugeGrid::FlashType::NONE);
        s2.setGaugeType(SpriteGaugeGrid::GaugeType::EXHARD);
    }
};

TEST_F(test_Graphics_SpriteGaugeGrid, valUpdate)
{
    using::testing::_;
    {
        gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, 74);
        s1.update(t0);
        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(0);    // clear light
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(37 - 3);    // normal light
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(11 - 1);    // clear dark
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(39 - 37 - 1); // normal dark

        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), Rect(0, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 2%
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), Rect(240, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 50%
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), Rect(360, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 74%
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), Rect(370, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 76%
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), Rect(490, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 100%
        s1.draw();
    }
    {
        gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, 33); // 16.5, floor to 16
        s1.update(t0);
        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(0);    // clear light
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(16 - 2);    // normal light
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(11 - 1);    // clear dark
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(39 - 16 - 2); // normal dark

        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), Rect(0, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 2%
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), Rect(150, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 32%
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), Rect(160, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 34%
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), Rect(370, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 76%
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), Rect(490, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 100%
        s1.draw();
    }
    {
        gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, 98);
        s1.update(t0);
        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(11 - 1 - 1);    // clear light
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(39 - 3);    // normal light
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1 - 1);    // clear dark
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(0); // normal dark

        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), Rect(0, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 2%
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), Rect(150, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 32%
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), Rect(370, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 76%
        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 40), Rect(430, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 88%
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), Rect(490, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 100%
        s1.draw();
    }
    {
        gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, 100);
        s2.update(t0);
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(45);
        EXPECT_CALL(*pt, draw(Rect(50, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(0);

        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), Rect(490, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 2%
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), Rect(250, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 50%
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), Rect(130, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 74%
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), Rect(120, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 76%
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), Rect(0, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 100%
        s2.draw();
    }
    {
        gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, 50);
        s2.update(t0);
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(25 - 2);
        EXPECT_CALL(*pt, draw(Rect(50, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(45 - (25 - 2));

        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), Rect(490, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 2%
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), Rect(250, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 50%
        EXPECT_CALL(*pt, draw(Rect(50, 0, 10, 40), Rect(130, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 74%
        EXPECT_CALL(*pt, draw(Rect(50, 0, 10, 40), Rect(120, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 76%
        EXPECT_CALL(*pt, draw(Rect(50, 0, 10, 40), Rect(0, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 100%
        s2.draw();
    }
}

#pragma endregion