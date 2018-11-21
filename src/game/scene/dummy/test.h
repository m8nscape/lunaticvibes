#pragma once

#include <SFML/Graphics.hpp>
#include <fmod.hpp>
#include <array>
#include "../../sound/sound.h"
#include "../scene.h"

namespace game
{
	class test : public vScene
	{
	public:
		test() = delete;
		test(std::shared_ptr<Sound> pSound);
		~test();
	private:
		sf::Font font;
		sf::Text text;
		void loadSprites() override;
		void loadSounds();
	protected:
		void mainLoop() override;
	public:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	};
}
