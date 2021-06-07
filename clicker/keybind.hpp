#pragma once

enum keybind_state: int
{
	ALWAYS = 0,
	HOLD,
	TOGGLE
};

class keybind
{
public:
	bool b_state = false;
	bool b_is_down = false;

	int i_mode = 0;
	int i_key = 0;

	keybind() = default;
	explicit keybind( bool state, int mode = 0, int key = 0 )
	{
		b_state = state;
		i_mode = mode;
		i_key = key;
	}

	inline auto get() -> bool
	{
		if ( i_key == 0 )
			return b_state;

		switch ( i_mode )
		{
			case 0:
				b_state = true;
				break;
			case 1:
				b_state = GetAsyncKeyState( i_key ) & 0x8000;
				break;

			case 2:
				auto h_state = GetAsyncKeyState( i_key );
				if ( h_state & 0x8000 )
					b_is_down = true;

				if ( b_is_down && !( h_state & 0x8000 ) )
				{
					b_state = !b_state;
					b_is_down = false;
				}
				break;
		}

		return b_state;
	}

	inline auto get_mode_as_string() const -> std::string
	{
		switch ( i_mode )
		{
			case 0: return std::string( "on" );
			case 1: return std::string( "hold" );
			case 2: return std::string( "toggle" );
			default: return {};
		}
	}
};