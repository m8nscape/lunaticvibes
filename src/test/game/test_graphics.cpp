#include "game/graphics/sprite.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class mock_Image : public Image
{
public:
    mock_Image() : Image("") {}
};

static Rect test_rect{ 0, 0, 256, 256 };
class mock_Texture : public Texture
{
public:
    mock_Texture() :Texture(mock_Image()) { _texRect = test_rect; _loaded = true; }
    MOCK_CONST_METHOD6(draw, void(const Rect&, Rect, const Color, const BlendMode, const bool, const double));
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
    EXPECT_EQ(d, Color(0x1020FFFF));
    EXPECT_EQ(d, Color(0x10, 0x0, 1234, 999));
    EXPECT_EQ(d, Color(0x10, -8124, 255, 255));
}

TEST(Graphics_Color, arithmetic)
{
    Color c{ 0x01020304 };
    Color d{ 0x05060708 };
    Color expected{ 0x06080A0C };
    EXPECT_EQ(c + d, expected);
    EXPECT_EQ(c * 2, d);
    //EXPECT_EQ(2 * c, d);
}


////////////////////////////////////////////////////////////////////////////////
// Render interface
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
    mock_Texture tex;
    pTexture pt{ std::make_shared<Texture>(tex) };
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
        ss1.appendKeyFrame({ 0, {Rect(0, 0, 255, 255), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss1.setLoopTime(0);
        ss1_1.appendKeyFrame({ 0, {Rect(0, 0, 255, 255), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss1_1.appendKeyFrame({ 255, {Rect(255, 255, 255, 255), RenderParams::CONSTANT, Color(0x00000000), BlendMode::ALPHA, 0, 0} });
        ss1_2.setLoopTime(0);
    }
};

TEST_F(test_Graphics_vSprite, func_update)
{
    gTimers.set(eTimer::K11_BOMB, 0);
    timestamp t(0), t1(128), t2(255), t3(256);

    ss1.update(t);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1.getCurrentRenderParams().rect, Rect(0, 0, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().angle, 0);

    ss1.update(t1);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1.getCurrentRenderParams().rect, Rect(0, 0, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().angle, 0);
    ss1_1.update(t1);
    ASSERT_TRUE(ss1_1._draw);
    ASSERT_EQ(ss1_1.getCurrentRenderParams().rect, Rect(128, 128, 255, 255));
    ASSERT_EQ(ss1_1.getCurrentRenderParams().color, Color(127, 127, 127, 127));
    ASSERT_EQ(ss1_1.getCurrentRenderParams().angle, 0);

    ss1.update(t2);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1.getCurrentRenderParams().rect, Rect(0, 0, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().angle, 0);
    ss1_1.update(t2);
    ASSERT_TRUE(ss1_1._draw);
    ASSERT_EQ(ss1_1.getCurrentRenderParams().rect, Rect(255, 255, 255, 255));
    ASSERT_EQ(ss1_1.getCurrentRenderParams().color, Color(0, 0, 0, 0));
    ASSERT_EQ(ss1_1.getCurrentRenderParams().angle, 0);

    ss1.update(t3);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1.getCurrentRenderParams().rect, Rect(0, 0, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1.getCurrentRenderParams().angle, 0);
    ss1_1.update(t3);
    ASSERT_FALSE(ss1_1._draw);
    ss1_2.update(t3);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1_2.getCurrentRenderParams().rect, Rect(0, 0, 255, 255));
    ASSERT_EQ(ss1_2.getCurrentRenderParams().color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1_2.getCurrentRenderParams().angle, 0);
}

////////////////////////////////////////////////////////////////////////////////
// Texture-split sprite:
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
    mock_Texture tex;
    pTexture pt{ std::make_shared<Texture>(tex) };
    mock_SpriteSelection s0{ pt, 1, 1, false };
    mock_SpriteSelection s{ pt, 2, 4, false };
    mock_SpriteSelection sv{ pt, 2, 4, true };
public:
    test_Graphics_SpriteSelection()
    {
        s0.setTimer(eTimer::K11_BOMB);
        s0.appendKeyFrame({ 0, {Rect(0, 0, 255, 255), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s0.setLoopTime(0);
        s.setTimer(eTimer::K11_BOMB);
        s.appendKeyFrame({ 0, {Rect(0, 0, 255, 255), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s.setLoopTime(0);
        sv.setTimer(eTimer::K11_BOMB);
        sv.appendKeyFrame({ 0, {Rect(0, 0, 255, 255), RenderParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sv.setLoopTime(0);
    }
};

TEST_F(test_Graphics_SpriteSelection, rectConstruct)
{
    EXPECT_EQ(s0._segments, 1);
    EXPECT_EQ(s0._texRect[0], test_rect);

    int w = 256 / 4;
    int h = 256 / 2;
    EXPECT_EQ(s._segments, 8);
    EXPECT_EQ(s._texRect[0], Rect(0 * w, 0 * h, w, h));
    EXPECT_EQ(s._texRect[3], Rect(3 * w, 0 * h, w, h));
    EXPECT_EQ(s._texRect[7], Rect(3 * w, 1 * h, w, h));

    EXPECT_EQ(sv._segments, 8);
    EXPECT_EQ(sv._texRect[0], Rect(0 * w, 0 * h, w, h));
    EXPECT_EQ(sv._texRect[3], Rect(1 * w, 1 * h, w, h));
    EXPECT_EQ(sv._texRect[7], Rect(1 * w, 3 * h, w, h));
}

////////////////////////////////////////////////////////////////////////////////
// Animated sprite: