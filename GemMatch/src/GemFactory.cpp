#include "Sprite.h"
#include "GemFactory.h"

const std::string GemFactory::PATH_RED_GEM = "assets/sprites/Red.png";
const std::string GemFactory::PATH_GREEN_GEM = "assets/sprites/Green.png";
const std::string GemFactory::PATH_BLUE_GEM = "assets/sprites/Blue.png";
const std::string GemFactory::PATH_PURPLE_GEM = "assets/sprites/Purple.png";
const std::string GemFactory::PATH_YELLOW_GEM = "assets/sprites/Yellow.png";

GemFactory::GemFactory()
{
	// create sprites for all gem colors
	Sprite* redSprite = new Sprite();
	redSprite->Load(PATH_RED_GEM);
	Sprite* greenSprite = new Sprite();
	greenSprite->Load(PATH_GREEN_GEM);
	Sprite* blueSprite = new Sprite();
	blueSprite->Load(PATH_BLUE_GEM);
	Sprite* purpleSprite = new Sprite();
	purpleSprite->Load(PATH_PURPLE_GEM);
	Sprite* yellowSprite = new Sprite();
	yellowSprite->Load(PATH_YELLOW_GEM);

	// add sprites to the map
	m_ColorSpriteMap[Gem::Color::Red] = redSprite;
	m_ColorSpriteMap[Gem::Color::Green] = greenSprite;
	m_ColorSpriteMap[Gem::Color::Blue] = blueSprite;
	m_ColorSpriteMap[Gem::Color::Purple] = purpleSprite;
	m_ColorSpriteMap[Gem::Color::Yellow] = yellowSprite;
}

GemFactory::~GemFactory()
{
	// clear the gem pool (gems are automatically destroyed due to unique_ptr)
	m_GemPool.clear();

	// delete sprites
	delete m_ColorSpriteMap[Gem::Color::Red];
	delete m_ColorSpriteMap[Gem::Color::Green];
	delete m_ColorSpriteMap[Gem::Color::Blue];
	delete m_ColorSpriteMap[Gem::Color::Purple];
	delete m_ColorSpriteMap[Gem::Color::Yellow];

	m_ColorSpriteMap.clear();

}

Gem* GemFactory::GetGem(Gem::Color color)
{
	// try to find an unused gem from the pool 
	for (const std::unique_ptr<Gem>& gem : m_GemPool)
	{
		if (gem->GetColor() == color && !gem->IsUsed())
		{
			// reset gem state
			gem->SetState(Gem::State::Normal);
			gem->SetUsed(true);
			return gem.get();
		}
	}

	// if there is no unused item create one
	std::unique_ptr<Gem> gem(new Gem(color));

	// assign corresponding sprite
	Sprite* sprite = m_ColorSpriteMap[color];
	gem->SetSprite(sprite);
	gem->SetUsed(true);

	// put gem into the pool
	m_GemPool.emplace_back(move(gem));

	return m_GemPool.back().get();
}

