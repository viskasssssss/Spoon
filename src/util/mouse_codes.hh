#pragma once

#include <cstdint>

namespace spoon
{
	using mouse_code = uint16_t;

	namespace mouse
	{
		enum : mouse_code
		{
			BUTTON_LEFT             = 1, /* SDL_BUTTON_LEFT */
			BUTTON_MIDDLE           = 2, /* SDL_BUTTON_MIDDLE */
			BUTTON_RIGHT            = 3, /* SDL_BUTTON_RIGHT */
			BUTTON_X1               = 4, /* SDL_BUTTON_X1 */
			BUTTON_X2               = 5, /* SDL_BUTTON_X2 */

			BUTTON_0                = BUTTON_LEFT,
			BUTTON_1                = BUTTON_MIDDLE,
			BUTTON_2                = BUTTON_RIGHT,
			BUTTON_3                = BUTTON_X1,
			BUTTON_4                = BUTTON_X2,
			BUTTON_5                = 6,
			BUTTON_6                = 7,
			BUTTON_7                = 8,

			BUTTON_LAST             = BUTTON_7
		};
	}
}
