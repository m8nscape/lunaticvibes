#include "test.h"
#include "../../input/gamepad.h"
#include "../../utils.h"

namespace game
{
	test::test(std::shared_ptr<Sound> pSound): vScene(pSound)
	{
		if (!font.loadFromFile("resources/sansation.ttf"))
			LOG_WARNING << "ERROR: Load font file failed!";
		text.setFont(font);
		text.setString("Push keys to play sounds (LS/LC, ZSXDCFV)");
		text.setCharacterSize(24);
		text.setFillColor(sf::Color::White);

		loadSounds();

		LOG_DEBUG << "test scene created";
	}

	test::~test()
	{
		LOG_DEBUG << "test scene destroyed";
	}

	void test::loadSprites()
	{
	}

	void test::loadSounds()
	{
		soundSystem->loadKeySample("resources/sound/kick_000.wav", 0);
		soundSystem->loadKeySample("resources/sound/kick_000.wav", 1);
		soundSystem->loadKeySample("resources/sound/cdefgab_000.wav", 2);
		soundSystem->loadKeySample("resources/sound/cdefgab_001.wav", 3);
		soundSystem->loadKeySample("resources/sound/cdefgab_002.wav", 4);
		soundSystem->loadKeySample("resources/sound/cdefgab_003.wav", 5);
		soundSystem->loadKeySample("resources/sound/cdefgab_004.wav", 6);
		soundSystem->loadKeySample("resources/sound/cdefgab_005.wav", 7);
		soundSystem->loadKeySample("resources/sound/cdefgab_006.wav", 8);
	}

	void test::mainLoop()
	{
		using keys = Input::gamepad::keys;
		for (size_t i = keys::S1L; i <= keys::K17; i++)
		{
			if (isGamepadKeyPressed(static_cast<keys>(i)))
				soundSystem->playKeySample(1, &i);
		}
	}

	void test::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(text, states);
	}

}