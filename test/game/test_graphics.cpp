#include "gmock/gmock.h"
#include "game/graphics/sprite.h"
using namespace lunaticvibes;

class mock_Image : public Image
{
public:
    mock_Image() : Image("") {}
};

static Rect TEST_RECT{ 0, 0, 256, 256 };
static Time t0{ 1 }, t1{ 2 }, t2{ 3 }, t3{ 4 }, t4{ 5 }, t5{ 6 }, t6{ 7 }, t7{ 8 };
class mock_Texture : public Texture
{
public:
    mock_Texture() :Texture(mock_Image()) { textureRect = TEST_RECT; loaded = true; }
    MOCK_CONST_METHOD6(draw, void(const Rect& srcRect, RectF dstRect,
        const Color c, const BlendMode blend, const bool filter, const double angleInDegrees));
    MOCK_CONST_METHOD7(draw, void(const Rect& srcRect, RectF dstRect,
        const Color c, const BlendMode blend, const bool filter, const double angleInDegrees, const Point& pt));
};


TEST(Color, construct)
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

TEST(Color, arithmetic)
{
    Color c{ 0x01020304 };
    Color d{ 0x05060708 };
    EXPECT_EQ(c + d, Color(0x06080A0C));
    EXPECT_EQ(c * 2, Color(0x02040608));
    EXPECT_EQ(c * -2, 0);
    //EXPECT_EQ(2 * c, d);
}


TEST(Rect, self_equal)
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

TEST(Rect, construct)
{
    EXPECT_EQ(Rect(0, 0, 40, 60), Rect(40, 60));

    Rect r{ 1, 2, 3, 4 };
    Rect r1(r);
    EXPECT_EQ(r, r1);
}

TEST(Rect, add_normal)
{
    Rect r1{ 0, 0, 40, 60 };
    Rect r2{ 0, 0, 40, 60 };
    Rect res = r1 + r2;

}

////////////////////////////////////////////////////////////////////////////////
// Render interface

#pragma region Render Interface (SpriteBase)
class mock_SpriteBase : public SpriteBase
{
public:
    mock_SpriteBase(const SpriteBuilder& builder) : SpriteBase(builder) {}
    MOCK_CONST_METHOD0(draw, void());
    FRIEND_TEST(test_SpriteBase, rectConstruct);
    FRIEND_TEST(test_SpriteBase, func_update);
};

class test_SpriteBase : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    SpriteBase::SpriteBuilder builder{ -1, pt };
    mock_SpriteBase ss1{ builder };
    mock_SpriteBase ss1_1{ builder };
    mock_SpriteBase ss1_2{ builder };
public:
    test_SpriteBase()
    {
        ss1.setMotionStartTimer(IndexTimer::K11_BOMB);
        ss1_1.setMotionStartTimer(IndexTimer::K11_BOMB);
        ss1_2.setMotionStartTimer(IndexTimer::K11_BOMB);

        ss1.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss1.setMotionLoopTo(0);

        ss1_1.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss1_1.appendMotionKeyFrame({ 255, {Rect(255, 255, 255, 255), MotionKeyFrameParams::CONSTANT, Color(0x00000000), BlendMode::ALPHA, 0, 0} });
        ss1_1.setMotionLoopTo(-1);

        ss1_2.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss1_2.appendMotionKeyFrame({ 255, {Rect(255, 255, 255, 255), MotionKeyFrameParams::CONSTANT, Color(0x00000000), BlendMode::ALPHA, 0, 0} });
        ss1_2.setMotionLoopTo(0);
    }
};

TEST_F(test_SpriteBase, func_update)
{
    State::set(IndexTimer::K11_BOMB, 0);
    Time t(0), t1(128), t2(255), t3(256), t4(512);

    ss1.update(t);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1._current.rect, RectF(0, 0, 0, 0));
    ASSERT_EQ(ss1._current.color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1._current.angle, 0);

    ss1.update(t1);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1._current.rect, RectF(0, 0, 0, 0));
    ASSERT_EQ(ss1._current.color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1._current.angle, 0);
    ss1_1.update(t1);
    ASSERT_TRUE(ss1_1._draw);
    ASSERT_EQ(ss1_1._current.rect, RectF(128, 128, 128, 128));
    ASSERT_EQ(ss1_1._current.color, Color(127, 127, 127, 127));
    ASSERT_EQ(ss1_1._current.angle, 0);

    ss1.update(t2);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1._current.rect, RectF(0, 0, 0, 0));
    ASSERT_EQ(ss1._current.color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1._current.angle, 0);
    ss1_1.update(t2);
    ASSERT_TRUE(ss1_1._draw);
    ASSERT_EQ(ss1_1._current.rect, RectF(255, 255, 255, 255));
    ASSERT_EQ(ss1_1._current.color, Color(0, 0, 0, 0));
    ASSERT_EQ(ss1_1._current.angle, 0);

    ss1.update(t3);
    ASSERT_TRUE(ss1._draw);
    ASSERT_EQ(ss1._current.rect, RectF(0, 0, 0, 0));
    ASSERT_EQ(ss1._current.color, Color(255, 255, 255, 255));
    ASSERT_EQ(ss1._current.angle, 0);
    ss1_1.update(t3);
    ASSERT_FALSE(ss1_1._draw);
    ss1_2.update(t3);
    ASSERT_TRUE(ss1_2._draw);
    ASSERT_EQ(ss1_2._current.rect, RectF(1, 1, 1, 1));
    ASSERT_EQ(ss1_2._current.color, Color(254, 254, 254, 254));
    ASSERT_EQ(ss1_2._current.angle, 0);

    ss1_2.update(t4);
    ASSERT_TRUE(ss1_2._draw);
    ASSERT_EQ(ss1_2._current.rect, RectF(2, 2, 2, 2));
    ASSERT_EQ(ss1_2._current.color, Color(253, 253, 253, 253));
    ASSERT_EQ(ss1_2._current.angle, 0);
}
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Texture-split sprite:

#pragma region Texture-split Sprite (SpriteSelection)
class mock_SpriteSelection : public SpriteSelection
{
public:
    mock_SpriteSelection(const SpriteSelection::SpriteSelectionBuilder& builder) : SpriteSelection(builder) {}
    FRIEND_TEST(sSelection, rectConstruct);
};

class sSelection : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteSelection s0{ SpriteSelection::SpriteSelectionBuilder{ -1, pt, RECT_FULL, 1, 1, false } };
    mock_SpriteSelection s { SpriteSelection::SpriteSelectionBuilder{ -1, pt, RECT_FULL, 2, 4, false } };
    mock_SpriteSelection sv{ SpriteSelection::SpriteSelectionBuilder{ -1, pt, RECT_FULL, 2, 4, true } };
public:
    sSelection()
    {
        s0.setMotionStartTimer(IndexTimer::K11_BOMB);
        s0.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s0.setMotionLoopTo(0);
        s.setMotionStartTimer(IndexTimer::K11_BOMB);
        s.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s.setMotionLoopTo(0);
        sv.setMotionStartTimer(IndexTimer::K11_BOMB);
        sv.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sv.setMotionLoopTo(0);
    }
};

TEST_F(sSelection, rectConstruct)
{
    EXPECT_EQ(s0.textureRects.size(), 1);
    EXPECT_EQ(s0.textureRects[0], TEST_RECT);

    int w = TEST_RECT.w / 4;
    int h = TEST_RECT.h / 2;
    EXPECT_EQ(s.textureRects.size(), 8);
    EXPECT_EQ(s.textureRects[0], Rect(0 * w, 0 * h, w, h));
    EXPECT_EQ(s.textureRects[1], Rect(1 * w, 0 * h, w, h));
    EXPECT_EQ(s.textureRects[2], Rect(2 * w, 0 * h, w, h));
    EXPECT_EQ(s.textureRects[3], Rect(3 * w, 0 * h, w, h));
    EXPECT_EQ(s.textureRects[4], Rect(0 * w, 1 * h, w, h));
    EXPECT_EQ(s.textureRects[5], Rect(1 * w, 1 * h, w, h));
    EXPECT_EQ(s.textureRects[6], Rect(2 * w, 1 * h, w, h));
    EXPECT_EQ(s.textureRects[7], Rect(3 * w, 1 * h, w, h));

    EXPECT_EQ(sv.textureRects.size(), 8);
    EXPECT_EQ(sv.textureRects[0], Rect(0 * w, 0 * h, w, h));
    EXPECT_EQ(sv.textureRects[1], Rect(0 * w, 1 * h, w, h));
    EXPECT_EQ(sv.textureRects[2], Rect(1 * w, 0 * h, w, h));
    EXPECT_EQ(sv.textureRects[3], Rect(1 * w, 1 * h, w, h));
    EXPECT_EQ(sv.textureRects[4], Rect(2 * w, 0 * h, w, h));
    EXPECT_EQ(sv.textureRects[5], Rect(2 * w, 1 * h, w, h));
    EXPECT_EQ(sv.textureRects[6], Rect(3 * w, 0 * h, w, h));
    EXPECT_EQ(sv.textureRects[7], Rect(3 * w, 1 * h, w, h));
}
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Animated sprite:

#pragma region Animated Sprite (SpriteAnimated)

class mock_SpriteAnimated : public SpriteAnimated
{
public:
    mock_SpriteAnimated(const SpriteAnimated::SpriteAnimatedBuilder& builder) : SpriteAnimated(builder) {}
    FRIEND_TEST(sAnimated, animRectConstruct);
    FRIEND_TEST(sAnimated, animUpdate);
};

class sAnimated : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteAnimated s{ SpriteAnimated::SpriteAnimatedBuilder{ -1, pt, RECT_FULL, 4, 2, false, 8, 8, IndexTimer::K11_BOMB}};
    mock_SpriteAnimated ss{ SpriteAnimated::SpriteAnimatedBuilder{ -1, pt, RECT_FULL, 8, 4, false, 8, 8, IndexTimer::K11_BOMB } };
public:
    sAnimated()
    {
        s.setMotionStartTimer(IndexTimer::K11_BOMB);
        s.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s.setMotionLoopTo(0);
        ss.setMotionStartTimer(IndexTimer::K11_BOMB);
        ss.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss.setMotionLoopTo(0);
    }
};


TEST_F(sAnimated, animRectConstruct)
{
    int w = TEST_RECT.w / 2;
    int h = TEST_RECT.h / 4;
    int ww = TEST_RECT.w / 4;
    int hh = TEST_RECT.h / 8;

    EXPECT_EQ(s.textureRects.size(), 1 * 8);
    EXPECT_EQ(s.animationFrames, 8);
    //EXPECT_EQ(s._aRect, Rect(0, 0, w, h));

    EXPECT_EQ(ss.textureRects.size(), 4 * 8);
    EXPECT_EQ(ss.animationFrames, 8);
    //EXPECT_EQ(ss._aRect, Rect(0, 0, w/2 , h/2));
    EXPECT_EQ(ss.textureRects[0], Rect(0, 0, ww, hh));
}

TEST_F(sAnimated, animUpdate)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    int w = TEST_RECT.w / 2;
    int h = TEST_RECT.h / 4;
    int ww = w / 2;
    int hh = h / 2;

    using namespace ::testing;
    //InSequence dummy;

    s.update(t0);
    EXPECT_CALL(*pt, draw(Rect(0 * w, 0 * h, w, h), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
    s.update(t1);
    EXPECT_CALL(*pt, draw(Rect(1 * w, 0 * h, w, h), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
    s.update(t2);
    EXPECT_CALL(*pt, draw(Rect(0 * w, 1 * h, w, h), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
    s.update(t3);
    EXPECT_CALL(*pt, draw(Rect(1 * w, 1 * h, w, h), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
    s.update(t4);
    EXPECT_CALL(*pt, draw(Rect(0 * w, 2 * h, w, h), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
    s.update(t5);
    EXPECT_CALL(*pt, draw(Rect(1 * w, 2 * h, w, h), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
    s.update(t6);
    EXPECT_CALL(*pt, draw(Rect(0 * w, 3 * h, w, h), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
    s.update(t7);
    EXPECT_CALL(*pt, draw(Rect(1 * w, 3 * h, w, h), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    ss.update(t0);
    EXPECT_CALL(*pt, draw(Rect(0 * ww, 0 * hh, ww, hh), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    ss.draw();
    ss.update(t1);
    EXPECT_CALL(*pt, draw(Rect(1 * ww, 0 * hh, ww, hh), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    ss.draw();
    ss.update(t2);
    EXPECT_CALL(*pt, draw(Rect(2 * ww, 0 * hh, ww, hh), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    ss.draw();
    ss.update(t3);
    EXPECT_CALL(*pt, draw(Rect(3 * ww, 0 * hh, ww, hh), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    ss.draw();
    ss.update(t4);
    EXPECT_CALL(*pt, draw(Rect(0 * ww, 1 * hh, ww, hh), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    ss.draw();
    ss.update(t5);
    EXPECT_CALL(*pt, draw(Rect(1 * ww, 1 * hh, ww, hh), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    ss.draw();
    ss.update(t6);
    EXPECT_CALL(*pt, draw(Rect(2 * ww, 1 * hh, ww, hh), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    ss.draw();
    ss.update(t7);
    EXPECT_CALL(*pt, draw(Rect(3 * ww, 1 * hh, ww, hh), RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    ss.draw();

}
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Number sprite:

#pragma region Animated number Sprite (SpriteNumber)

class mock_SpriteNumber : public SpriteNumber
{
public:
    mock_SpriteNumber(const SpriteNumber::SpriteNumberBuilder& builder): SpriteNumber(builder) {}

    FRIEND_TEST(sNumber, construct);
    FRIEND_TEST(sNumber, num_1);
    FRIEND_TEST(sNumber, num_23);
    FRIEND_TEST(sNumber, num_456);
    FRIEND_TEST(sNumber, num_1234);
    FRIEND_TEST(sNumber, num_0);
    FRIEND_TEST(sNumber, num_norm_9999);
    FRIEND_TEST(sNumber, num_norm_m1);
    FRIEND_TEST(sNumber, num_full_m65532);
    FRIEND_TEST(sNumber, num_full_0);
    FRIEND_TEST(sNumber, animUpdate);
};

class sNumber : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    Rect dstRect{ 0,0,10,10 };
    Color dstColor{ 0xFFFFFFFF };
    mock_SpriteNumber s1  {SpriteNumber::SpriteNumberBuilder{ -1, pt, Rect(0, 0, 100, 160), 2, 5,  false, 1, 0, IndexTimer::SCENE_START, NumberAlign::NUM_ALIGN_LEFT,    1, IndexNumber::_TEST1}};  // 20,80
    mock_SpriteNumber s   {SpriteNumber::SpriteNumberBuilder{ -1, pt, Rect(0, 0, 100, 160), 2, 5,  false, 1, 0, IndexTimer::SCENE_START, NumberAlign::NUM_ALIGN_LEFT,    4, IndexNumber::_TEST1}};   // 20,80
    mock_SpriteNumber sa  {SpriteNumber::SpriteNumberBuilder{ -1, pt, Rect(0, 0, 100, 160), 4, 5,  false, 2, 2, IndexTimer::K11_BOMB,    NumberAlign::NUM_ALIGN_LEFT,    4, IndexNumber::_TEST1}};     // 20,40
    mock_SpriteNumber s11 {SpriteNumber::SpriteNumberBuilder{ -1, pt, Rect(0, 0, 110, 160), 1, 11, false, 1, 0, IndexTimer::SCENE_START, NumberAlign::NUM_ALIGN_LEFT,    4, IndexNumber::_TEST1}};    // 10,160
    mock_SpriteNumber sa11{SpriteNumber::SpriteNumberBuilder{ -1, pt, Rect(0, 0, 110, 160), 4, 11, false, 4, 4, IndexTimer::K11_BOMB,    NumberAlign::NUM_ALIGN_LEFT,    4, IndexNumber::_TEST1}};  // 10,40
    mock_SpriteNumber s24 {SpriteNumber::SpriteNumberBuilder{ -1, pt, Rect(0, 0, 240, 160), 2, 12, false, 1, 0, IndexTimer::SCENE_START, NumberAlign::NUM_ALIGN_LEFT,    4, IndexNumber::_TEST1}};    // 20,80
    mock_SpriteNumber sa24{SpriteNumber::SpriteNumberBuilder{ -1, pt, Rect(0, 0, 240, 160), 8, 12, false, 4, 4, IndexTimer::K11_BOMB,    NumberAlign::NUM_ALIGN_LEFT,    4, IndexNumber::_TEST1}};  // 20,30
    mock_SpriteNumber sr  {SpriteNumber::SpriteNumberBuilder{ -1, pt, Rect(0, 0, 110, 160), 1, 11, false, 1, 0, IndexTimer::SCENE_START, NumberAlign::NUM_ALIGN_RIGHT,   4, IndexNumber::_TEST1}};    // 10,160
    mock_SpriteNumber sc  {SpriteNumber::SpriteNumberBuilder{ -1, pt, Rect(0, 0, 110, 160), 1, 11, false, 1, 0, IndexTimer::SCENE_START, NumberAlign::NUM_ALIGN_CENTER,  4, IndexNumber::_TEST1}};   // 10,160
public:
    sNumber()
    {
        s1.setMotionStartTimer(IndexTimer::K11_BOMB);
        s1.appendMotionKeyFrame({ 0, {dstRect, MotionKeyFrameParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        s1.setMotionLoopTo(0);
        s.setMotionStartTimer(IndexTimer::K11_BOMB);
        s.appendMotionKeyFrame({ 0, {dstRect, MotionKeyFrameParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        s.setMotionLoopTo(0);
        sa.setMotionStartTimer(IndexTimer::K11_BOMB);
        sa.appendMotionKeyFrame({ 0, {dstRect, MotionKeyFrameParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        sa.setMotionLoopTo(0);

        s11.setMotionStartTimer(IndexTimer::K11_BOMB);
        s11.appendMotionKeyFrame({ 0, {dstRect, MotionKeyFrameParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        s11.setMotionLoopTo(0);
        sa11.setMotionStartTimer(IndexTimer::K11_BOMB);
        sa11.appendMotionKeyFrame({ 0, {dstRect, MotionKeyFrameParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        sa11.setMotionLoopTo(0);
        s24.setMotionStartTimer(IndexTimer::K11_BOMB);
        s24.appendMotionKeyFrame({ 0, {dstRect, MotionKeyFrameParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        s24.setMotionLoopTo(0);
        sa24.setMotionStartTimer(IndexTimer::K11_BOMB);
        sa24.appendMotionKeyFrame({ 0, {dstRect, MotionKeyFrameParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        sa24.setMotionLoopTo(0);

        sr.setMotionStartTimer(IndexTimer::K11_BOMB);
        sr.appendMotionKeyFrame({ 0, {dstRect, MotionKeyFrameParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        sr.setMotionLoopTo(0);
        sc.setMotionStartTimer(IndexTimer::K11_BOMB);
        sc.appendMotionKeyFrame({ 0, {dstRect, MotionKeyFrameParams::CONSTANT, dstColor, BlendMode::ALPHA, 0, 0} });
        sc.setMotionLoopTo(0);
    }
};

TEST_F(sNumber, construct)
{
    EXPECT_EQ(s1.maxDigits, 1);
    EXPECT_EQ(s1.numberType, NumberType::NUM_TYPE_NORMAL);
    EXPECT_EQ(s.maxDigits, 4);
    EXPECT_EQ(s.numberType, NumberType::NUM_TYPE_NORMAL);
    EXPECT_EQ(sa11.maxDigits, 4);
    EXPECT_EQ(sa11.numberType, NumberType::NUM_TYPE_BLANKZERO);
    EXPECT_EQ(sa24.maxDigits, 4);
    EXPECT_EQ(sa24.numberType, NumberType::NUM_TYPE_FULL);
}

TEST_F(sNumber, num_1)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    State::set(IndexNumber::_TEST1, 1);
    s.update(t0);
    EXPECT_THAT(s.digitNumber, ElementsAre(1, -1, -1, -1));
    sa11.update(t0);
    EXPECT_EQ(sa11.digitNumber[0], 1);
    sa24.update(t0);
    EXPECT_EQ(sa24.digitNumber[0], 1);
    EXPECT_EQ(sa24.digitNumber[1], NUM_FULL_PLUS);
}
TEST_F(sNumber, num_23)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    State::set(IndexNumber::_TEST1, 23);
    s.update(t0);
    EXPECT_THAT(s.digitNumber, ElementsAre(3, 2, -1, -1));
    sa11.update(t0);
    EXPECT_EQ(sa11.digitNumber[0], 3);
    EXPECT_EQ(sa11.digitNumber[1], 2);
    sa24.update(t0);
    EXPECT_EQ(sa24.digitNumber[0], 3);
    EXPECT_EQ(sa24.digitNumber[1], 2);
    EXPECT_EQ(sa24.digitNumber[2], NUM_FULL_PLUS);
}
TEST_F(sNumber, num_456)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;


    State::set(IndexNumber::_TEST1, 456);
    s.update(t0);
    EXPECT_THAT(s.digitNumber, ElementsAre(6, 5, 4, -1));
    sa11.update(t0);
    EXPECT_EQ(sa11.digitNumber[0], 6);
    EXPECT_EQ(sa11.digitNumber[1], 5);
    EXPECT_EQ(sa11.digitNumber[2], 4);
    sa24.update(t0);
    EXPECT_THAT(sa24.digitNumber, ElementsAre(6, 5, 4, NUM_FULL_PLUS));
}
TEST_F(sNumber, num_1234)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;


    State::set(IndexNumber::_TEST1, 1234);
    s.update(t0);
    EXPECT_THAT(s.digitNumber, ElementsAre(4, 3, 2, 1));
    sa11.update(t0);
    EXPECT_THAT(sa11.digitNumber, ElementsAre(4, 3, 2, 1));
    sa24.update(t0);
    EXPECT_THAT(sa24.digitNumber, ElementsAre(4, 3, 2, NUM_FULL_PLUS));
}
TEST_F(sNumber, num_0)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;


    State::set(IndexNumber::_TEST1, 0);
    s.update(t0);
    EXPECT_THAT(s.digitNumber, ElementsAre(0, -1, -1, -1));
    sa11.update(t0);
    EXPECT_EQ(sa11.digitNumber[0], 0);
    sa24.update(t0);
    EXPECT_EQ(sa24.digitNumber[0], 0);
    EXPECT_EQ(sa24.digitNumber[1], NUM_FULL_PLUS);
}

TEST_F(sNumber, num_norm_9999)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;


    State::set(IndexNumber::_TEST1, 9999);
    s.update(t0);
    EXPECT_THAT(s.digitNumber, ElementsAre(9, 9, 9, 9));
}

TEST_F(sNumber, num_norm_m1)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;


    State::set(IndexNumber::_TEST1, 2147483647);
    s.update(t0);
    EXPECT_THAT(s.digitNumber, ElementsAre(7, 4, 6, 3));
}

TEST_F(sNumber, num_full_m65532)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;


    State::set(IndexNumber::_TEST1, -65532);
    s.update(t0);
    EXPECT_THAT(s.digitNumber, ElementsAre(2, 3, 5, 5));
    sa11.update(t0);
    EXPECT_THAT(sa11.digitNumber, ElementsAre(2, 3, 5, 5));
    sa24.update(t0);
    EXPECT_THAT(sa24.digitNumber, ElementsAre(12+2, 12+3, 12+5, NUM_FULL_MINUS));
}

TEST_F(sNumber, num_full_0)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;


    State::set(IndexNumber::_TEST1, 0);
    s.update(t0);
    EXPECT_THAT(s.digitNumber, ElementsAre(0, -1, -1, -1));
    sa11.update(t0);
    EXPECT_EQ(sa11.digitNumber[0], 0);
    sa24.update(t0);
    EXPECT_EQ(sa24.digitNumber[0], 0);
    EXPECT_EQ(sa24.digitNumber[1], NUM_FULL_PLUS);
}

TEST_F(sNumber, rect_normal_1)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());

    State::set(IndexNumber::_TEST1, 1);
    s1.update(t0);

    EXPECT_CALL(*pt, draw(Rect(20, 0, 20, 80), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
    s1.draw();

}


TEST_F(sNumber, rect_normal_4)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 456);
        s.update(t0);

        EXPECT_CALL(*pt, draw(Rect(20, 80, 20, 80), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(0, 80, 20, 80),  RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 0, 20, 80),  RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        //EXPECT_CALL(*pt, draw(Rect(0, 0, 20, 80), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        s.draw();
    }
    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 1234);
        s.update(t0);

        EXPECT_CALL(*pt, draw(Rect(80, 0, 20, 80), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(60, 0, 20, 80), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(40, 0, 20, 80), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 0, 20, 80), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        s.draw();
    }

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 6789);
        s.update(t0);

        EXPECT_CALL(*pt, draw(Rect(80, 80, 20, 80), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(60, 80, 20, 80), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(40, 80, 20, 80), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 80, 20, 80), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        s.draw();
    }

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 0);
        s.update(t0);

        EXPECT_CALL(*pt, draw(Rect(0, 0, 20, 80), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        //EXPECT_CALL(*pt, draw(Rect(0, 0, 20, 80), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        //EXPECT_CALL(*pt, draw(Rect(0, 0, 20, 80), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        //EXPECT_CALL(*pt, draw(Rect(0, 0, 20, 80), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        s.draw();
    }
}


TEST_F(sNumber, rect_bzero_4)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 123);
        s11.update(t0);

        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 160), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 160), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 160), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        //EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        s11.draw();
    }

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 6789);
        s11.update(t0);

        EXPECT_CALL(*pt, draw(Rect(90, 0, 10, 160), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 0, 10, 160), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(70, 0, 10, 160), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(60, 0, 10, 160), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        s11.draw();
    }

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 0);
        s11.update(t0);

        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 160), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);     //0

        //EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        //EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        //EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        s11.draw();
    }
}



TEST_F(sNumber, rect_full_4)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 123);
        s24.update(t0);

        EXPECT_CALL(*pt, draw(Rect(60, 0, 20, 80), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 3
        EXPECT_CALL(*pt, draw(Rect(40, 0, 20, 80), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 2
        EXPECT_CALL(*pt, draw(Rect(20, 0, 20, 80), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 1
        EXPECT_CALL(*pt, draw(Rect(220, 0, 20, 80), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // +
        s24.draw();
    }

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 6789);
        s24.update(t0);

        EXPECT_CALL(*pt, draw(Rect(180, 0, 20, 80), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 9
        EXPECT_CALL(*pt, draw(Rect(160, 0, 20, 80), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 8
        EXPECT_CALL(*pt, draw(Rect(140, 0, 20, 80), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1); // 7
        EXPECT_CALL(*pt, draw(Rect(220, 0, 20, 80), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  // +
        s24.draw();
    }

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, -20);
        s24.update(t0);

        EXPECT_CALL(*pt, draw(Rect(0, 80, 20, 80), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);    // 0
        EXPECT_CALL(*pt, draw(Rect(40, 80, 20, 80), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);   // 2
        EXPECT_CALL(*pt, draw(Rect(220, 80, 20, 80), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);   // -

        //EXPECT_CALL(*pt, draw(Rect(200, 80, 20, 80), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  // b
        s24.draw();
    }
}


TEST_F(sNumber, rect_bzero_4_right)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 123);
        sr.update(t0);

        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 160), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 160), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 160), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sr.draw();
    }

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 6789);
        sr.update(t0);

        EXPECT_CALL(*pt, draw(Rect(90, 0, 10, 160), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 0, 10, 160), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(70, 0, 10, 160), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(60, 0, 10, 160), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sr.draw();
    }

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 0);
        sr.update(t0);

        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 160), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(100, 0, 10, 160), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sr.draw();
    }
}

TEST_F(sNumber, rect_bzero_4_anim_1_123)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    // time: 1
    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 123);
        sa11.update(t1);

        EXPECT_CALL(*pt, draw(Rect(30, 40, 10, 40), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 40, 10, 40), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(10, 40, 10, 40), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        //EXPECT_CALL(*pt, draw(Rect(100, 40, 10, 40), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        sa11.draw();
    }

}
TEST_F(sNumber, rect_bzero_4_anim_1_6789)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 6789);
        sa11.update(t1);

        EXPECT_CALL(*pt, draw(Rect(90, 40, 10, 40), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 40, 10, 40), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(70, 40, 10, 40), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(60, 40, 10, 40), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sa11.draw();
    }

}
TEST_F(sNumber, rect_bzero_4_anim_1_0)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 0);
        sa11.update(t1);

        EXPECT_CALL(*pt, draw(Rect(0, 40, 10, 40), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        //EXPECT_CALL(*pt, draw(Rect(100, 40, 10, 40), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        //EXPECT_CALL(*pt, draw(Rect(100, 40, 10, 40), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        //EXPECT_CALL(*pt, draw(Rect(100, 40, 10, 40), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        sa11.draw();
    }

}
TEST_F(sNumber, rect_bzero_4_anim_3_123)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    //time: 3
    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 123);
        sa11.update(t3);

        EXPECT_CALL(*pt, draw(Rect(30, 120, 10, 40), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(20, 120, 10, 40), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(10, 120, 10, 40), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        //EXPECT_CALL(*pt, draw(Rect(100, 40, 10, 40), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        sa11.draw();
    }

}
TEST_F(sNumber, rect_bzero_4_anim_3_6789)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 6789);
        sa11.update(t3);

        EXPECT_CALL(*pt, draw(Rect(90, 120, 10, 40), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(80, 120, 10, 40), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(70, 120, 10, 40), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        EXPECT_CALL(*pt, draw(Rect(60, 120, 10, 40), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        sa11.draw();
    }

}
TEST_F(sNumber, rect_bzero_4_anim_3_0)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;

    {
        //InSequence dummy;
        State::set(IndexNumber::_TEST1, 0);
        sa11.update(t3);

        EXPECT_CALL(*pt, draw(Rect(0, 120, 10, 40), RectF(0, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);
        //EXPECT_CALL(*pt, draw(Rect(100, 40, 10, 40), RectF(30, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        //EXPECT_CALL(*pt, draw(Rect(100, 40, 10, 40), RectF(20, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
        //EXPECT_CALL(*pt, draw(Rect(100, 40, 10, 40), RectF(10, 0, 10, 10), dstColor, BlendMode::ALPHA, 0, 0)).Times(1);  //b
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
    mock_SpriteSlider(const SpriteSlider::SpriteSliderBuilder& builder) : SpriteSlider(builder) {}
};

class sSlider : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteSlider sL{ SpriteSlider::SpriteSliderBuilder { -1, pt, TEST_RECT, 1, 1, false, 1, 0, IndexTimer::K11_BOMB, SliderDirection::LEFT,  IndexSlider::_TEST1, 101, } };
    mock_SpriteSlider sR{ SpriteSlider::SpriteSliderBuilder { -1, pt, TEST_RECT, 1, 1, false, 1, 0, IndexTimer::K11_BOMB, SliderDirection::RIGHT, IndexSlider::_TEST1, 101, } };
    mock_SpriteSlider sU{ SpriteSlider::SpriteSliderBuilder { -1, pt, TEST_RECT, 1, 1, false, 1, 0, IndexTimer::K11_BOMB, SliderDirection::UP,    IndexSlider::_TEST1, 201, } };
    mock_SpriteSlider sD{ SpriteSlider::SpriteSliderBuilder { -1, pt, TEST_RECT, 1, 1, false, 1, 0, IndexTimer::K11_BOMB, SliderDirection::DOWN,  IndexSlider::_TEST1, 201, } };
public:
    sSlider()
    {
        sL.setMotionStartTimer(IndexTimer::K11_BOMB);
        sL.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sL.setMotionLoopTo(0);
        sR.setMotionStartTimer(IndexTimer::K11_BOMB);
        sR.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sR.setMotionLoopTo(0);
        sU.setMotionStartTimer(IndexTimer::K11_BOMB);
        sU.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sU.setMotionLoopTo(0);
        sD.setMotionStartTimer(IndexTimer::K11_BOMB);
        sD.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sD.setMotionLoopTo(0);
    }
};

TEST_F(sSlider, updateLeft)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sL;

    //InSequence dummy;
    State::set(IndexSlider::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0 - 33, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 0.50);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0 - 50, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 1.00);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0 - 100, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
}

TEST_F(sSlider, updateRight)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sR;

    //InSequence dummy;
    State::set(IndexSlider::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0 + 33, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 0.5);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0 + 50, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 1);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0 + 100, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
}

TEST_F(sSlider, updateUp)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sU;

    //InSequence dummy;
    State::set(IndexSlider::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0 - 66, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 0.50);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0 - 100, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 1.00);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0 - 200, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
}

TEST_F(sSlider, updateDown)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sD;

    //InSequence dummy;
    State::set(IndexSlider::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0 + 66, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 0.505);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0 + 101, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexSlider::_TEST1, 1.00);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0 + 200, 0, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
}
#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// Bargraph sprite:

#pragma region Bargraph Sprite (SpriteBargraph)
class mock_SpritIndexBargraph : public SpriteBargraph
{
public:
    mock_SpritIndexBargraph(const SpriteBargraph::SpriteBargraphBuilder& builder) : SpriteBargraph(builder) {}
};

class sBargraph : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpritIndexBargraph sL{ SpriteBargraph::SpriteBargraphBuilder{ -1, pt, TEST_RECT, 1, 1, false, 1, 0, IndexTimer::K11_BOMB, BargraphDirection::LEFT,  IndexBargraph::_TEST1 } };
    mock_SpritIndexBargraph sR{ SpriteBargraph::SpriteBargraphBuilder{ -1, pt, TEST_RECT, 1, 1, false, 1, 0, IndexTimer::K11_BOMB, BargraphDirection::RIGHT, IndexBargraph::_TEST1 } };
    mock_SpritIndexBargraph sU{ SpriteBargraph::SpriteBargraphBuilder{ -1, pt, TEST_RECT, 1, 1, false, 1, 0, IndexTimer::K11_BOMB, BargraphDirection::UP,    IndexBargraph::_TEST1 } };
    mock_SpritIndexBargraph sD{ SpriteBargraph::SpriteBargraphBuilder{ -1, pt, TEST_RECT, 1, 1, false, 1, 0, IndexTimer::K11_BOMB, BargraphDirection::DOWN,  IndexBargraph::_TEST1 } };
public:
    sBargraph()
    {
        sL.setMotionStartTimer(IndexTimer::K11_BOMB);
        sL.appendMotionKeyFrame({ 0, {Rect(0, 0, 200, 200), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sL.setMotionLoopTo(0);
        sR.setMotionStartTimer(IndexTimer::K11_BOMB);
        sR.appendMotionKeyFrame({ 0, {Rect(0, 0, 200, 200), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sR.setMotionLoopTo(0);
        sU.setMotionStartTimer(IndexTimer::K11_BOMB);
        sU.appendMotionKeyFrame({ 0, {Rect(0, 0, 200, 200), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sU.setMotionLoopTo(0);
        sD.setMotionStartTimer(IndexTimer::K11_BOMB);
        sD.appendMotionKeyFrame({ 0, {Rect(0, 0, 200, 200), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        sD.setMotionLoopTo(0);
    }
};

TEST_F(sBargraph, updateLeft)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sL;

    //InSequence dummy;
    State::set(IndexBargraph::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(200, 0, 0, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(200 - 66, 0, 66, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 0.50);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(200 - 100, 0, 100, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 1.00);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(200 - 200, 0, 200, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
}

TEST_F(sBargraph, updateRight)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sR;

    //InSequence dummy;
    State::set(IndexBargraph::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 0, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 66, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 0.5);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 100, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 1);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 200, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
}

TEST_F(sBargraph, updateUp)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sU;

    //InSequence dummy;
    State::set(IndexBargraph::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 200, 200, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 134, 200, 66), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 0.50);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 100, 200, 100), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 1.00);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 200, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
}

TEST_F(sBargraph, updateDown)
{
    State::set(IndexTimer::K11_BOMB, t0.norm());
    using namespace ::testing;
    auto& s = sD;

    //InSequence dummy;
    State::set(IndexBargraph::_TEST1, 0);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 200, 0), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 0.33);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 200, 66), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 0.505);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 200, 101), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();

    State::set(IndexBargraph::_TEST1, 1.00);
    s.update(t0);
    EXPECT_CALL(*pt, draw(TEST_RECT, RectF(0, 0, 200, 200), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0, Point(0, 0))).Times(1);
    s.draw();
}
#pragma endregion


////////////////////////////////////////////////////////////////////////////////
// Option sprite:

#pragma region Option Sprite (SpriteOption)

class mock_SpriteOption : public SpriteOption
{
public:
    mock_SpriteOption(const SpriteOption::SpriteOptionBuilder& builder) : SpriteOption(builder) {}
    FRIEND_TEST(sOption, switchTest);
    FRIEND_TEST(sOption, optionTest);
};

class sOption : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteOption ss  { SpriteOption::SpriteOptionBuilder { -1, pt, RECT_FULL, 1, 2, false, 1, 1, IndexTimer::K11_BOMB } };
    mock_SpriteOption ss0 { SpriteOption::SpriteOptionBuilder { -1, pt, RECT_FULL, 1, 1, false, 1, 1, IndexTimer::K11_BOMB } };
    mock_SpriteOption so  { SpriteOption::SpriteOptionBuilder { -1, pt, RECT_FULL, 2, 3, false, 1, 1, IndexTimer::K11_BOMB } };
public:
    sOption()
    {
        ss.setMotionStartTimer(IndexTimer::K11_BOMB);
        ss.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss.setMotionLoopTo(0);
        ss.setInd(SpriteOption::opType::SWITCH, (unsigned)IndexSwitch::_TEST1);
        ss0.setMotionStartTimer(IndexTimer::K11_BOMB);
        ss0.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        ss0.setMotionLoopTo(0);
        ss0.setInd(SpriteOption::opType::SWITCH, (unsigned)IndexSwitch::_TEST1);
        so.setMotionStartTimer(IndexTimer::K11_BOMB);
        so.appendMotionKeyFrame({ 0, {Rect(0, 0, 0, 0), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        so.setMotionLoopTo(0);
        so.setInd(SpriteOption::opType::OPTION, (unsigned)IndexOption::_TEST1);
    }
};

TEST_F(sOption, switchTest)
{
    ASSERT_EQ(ss.indType, SpriteOption::opType::SWITCH);
    ASSERT_EQ(ss0.indType, SpriteOption::opType::SWITCH);

    State::set(IndexTimer::K11_BOMB, t0.norm());

    State::set(IndexSwitch::_TEST1, false);
    ss.update(t0);
    EXPECT_EQ(ss.selectionIndex, 0);
    State::set(IndexSwitch::_TEST1, true);
    ss.update(t0);
    EXPECT_EQ(ss.selectionIndex, 1);
    ss0.update(t0);
    EXPECT_EQ(ss0.selectionIndex, 0);
}


TEST_F(sOption, optionTest)
{
    ASSERT_EQ(so.indType, SpriteOption::opType::OPTION);

    State::set(IndexTimer::K11_BOMB, t0.norm());

    State::set(IndexOption::_TEST1, 0);
    so.update(t0);
    EXPECT_EQ(so.selectionIndex, 0);
    State::set(IndexOption::_TEST1, 1);
    so.update(t0);
    EXPECT_EQ(so.selectionIndex, 1);
    State::set(IndexOption::_TEST1, 5);
    so.update(t0);
    EXPECT_EQ(so.selectionIndex, 5);
    State::set(IndexOption::_TEST1, 12);
    so.update(t0);
    EXPECT_EQ(so.selectionIndex, 5);

}

#pragma endregion

////////////////////////////////////////////////////////////////////////////////
// GaugeGrid sprite:

#pragma region Grid type gauge Sprite (SpriteGaugeGrid)
class mock_SpriteGaugeGrid : public SpriteGaugeGrid
{
public:
    mock_SpriteGaugeGrid(const SpriteGaugeGrid::SpriteGaugeGridBuilder& builder) : SpriteGaugeGrid(builder) {}
};

class sGaugeGrid : public ::testing::Test
{
protected:
    std::shared_ptr<mock_Texture> pt{ std::make_shared<mock_Texture>() };
    mock_SpriteGaugeGrid s1{ SpriteGaugeGrid::SpriteGaugeGridBuilder { -1, pt, Rect(0, 0, 40, 40), 1, 4, false, 1, 0, IndexTimer::K11_BOMB,  10, 0, 0, 100, 50, IndexNumber::PLAY_1P_GROOVEGAUGE } };
    mock_SpriteGaugeGrid s2{ SpriteGaugeGrid::SpriteGaugeGridBuilder { -1, pt, Rect(0, 0, 60, 40), 1, 6, false, 1, 0, IndexTimer::K11_BOMB, -10, 0, 0, 100, 50, IndexNumber::PLAY_2P_GROOVEGAUGE } };
public:
    sGaugeGrid()
    {
        s1.setMotionStartTimer(IndexTimer::K11_BOMB);
        s1.appendMotionKeyFrame({ 0, {Rect(0, 0, 10, 40), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s1.setMotionLoopTo(0);
        s1.setFlashType(SpriteGaugeGrid::FlashType::NONE);
        s1.setGaugeType(SpriteGaugeGrid::GaugeType::GROOVE);

        s2.setMotionStartTimer(IndexTimer::K11_BOMB);
        s2.appendMotionKeyFrame({ 0, {Rect(490, 100, 10, 40), MotionKeyFrameParams::CONSTANT, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0} });
        s2.setMotionLoopTo(0);
        s2.setFlashType(SpriteGaugeGrid::FlashType::NONE);
        s2.setGaugeType(SpriteGaugeGrid::GaugeType::EX_SURVIVAL);
    }
};

TEST_F(sGaugeGrid, valUpdate)
{
    State::set(IndexTimer::K11_BOMB, 0);
    using::testing::_;
    {
        State::set(IndexNumber::PLAY_1P_GROOVEGAUGE, 74);
        s1.update(t0);
        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(0);    // clear light
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(37 - 3);    // normal light
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(11 - 1);    // clear dark
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(39 - 37 - 1); // normal dark

        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), RectF(0, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 2%
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), RectF(240, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 50%
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), RectF(360, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 74%
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), RectF(370, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 76%
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), RectF(490, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 100%
        s1.draw();
    }
    {
        State::set(IndexNumber::PLAY_1P_GROOVEGAUGE, 33); // 16.5, floor to 16
        s1.update(t0);
        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(0);    // clear light
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(16 - 2);    // normal light
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(11 - 1);    // clear dark
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(39 - 16 - 2); // normal dark

        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), RectF(0, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 2%
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), RectF(150, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 32%
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), RectF(160, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 34%
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), RectF(370, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 76%
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), RectF(490, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 100%
        s1.draw();
    }
    {
        State::set(IndexNumber::PLAY_1P_GROOVEGAUGE, 98);
        s1.update(t0);
        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(11 - 1 - 1);    // clear light
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(39 - 3);    // normal light
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(1 - 1);    // clear dark
        EXPECT_CALL(*pt, draw(Rect(30, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(0); // normal dark

        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), RectF(0, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 2%
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), RectF(150, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 32%
        EXPECT_CALL(*pt, draw(Rect(10, 0, 10, 40), RectF(370, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 76%
        EXPECT_CALL(*pt, draw(Rect(0, 0, 10, 40), RectF(430, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 88%
        EXPECT_CALL(*pt, draw(Rect(20, 0, 10, 40), RectF(490, 0, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 100%
        s1.draw();
    }
    {
        State::set(IndexNumber::PLAY_2P_GROOVEGAUGE, 100);
        s2.update(t0);
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(45);
        EXPECT_CALL(*pt, draw(Rect(50, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(0);

        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), RectF(490, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 2%
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), RectF(250, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 50%
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), RectF(130, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 74%
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), RectF(120, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 76%
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), RectF(0, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 100%
        s2.draw();
    }
    {
        State::set(IndexNumber::PLAY_2P_GROOVEGAUGE, 50);
        s2.update(t0);
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(25 - 2);
        EXPECT_CALL(*pt, draw(Rect(50, 0, 10, 40), _, Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0)).Times(45 - (25 - 2));

        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), RectF(490, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 2%
        EXPECT_CALL(*pt, draw(Rect(40, 0, 10, 40), RectF(250, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 50%
        EXPECT_CALL(*pt, draw(Rect(50, 0, 10, 40), RectF(130, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 74%
        EXPECT_CALL(*pt, draw(Rect(50, 0, 10, 40), RectF(120, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 76%
        EXPECT_CALL(*pt, draw(Rect(50, 0, 10, 40), RectF(0, 100, 10, 40), Color(0xFFFFFFFF), BlendMode::ALPHA, 0, 0));    // 100%
        s2.draw();
    }
}

#pragma endregion
