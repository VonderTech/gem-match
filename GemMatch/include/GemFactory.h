#ifndef __GEM_FACTORY_H__
#define __GEM_FACTORY_H__

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>
#include "Gem.h"

class Sprite;
class Gem;

/*! Produces and recycles gems.
*	Implementation of the Flyweight pattern: all gems of the same color share the same sprite.
*	If gems are no longer used they get recycled and can be used again.
*/
class GemFactory
{
public:
	GemFactory();
	virtual ~GemFactory();

	/*! Retrieves a pointer to a gem of the specified color */
	Gem* GetGem(Gem::Color color);

private:
	// map that contains all sprites
	std::unordered_map <Gem::Color, Sprite*> m_ColorSpriteMap;
	// pool that holds all gems
	std::vector<std::unique_ptr<Gem>> m_GemPool;

	static const std::string PATH_RED_GEM;
	static const std::string PATH_GREEN_GEM;
	static const std::string PATH_BLUE_GEM;
	static const std::string PATH_PURPLE_GEM;
	static const std::string PATH_YELLOW_GEM;
};

#endif

