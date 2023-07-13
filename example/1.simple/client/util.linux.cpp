#include "pch.h"

#if defined(__linux__)
#include <termios.h>  

class _keyboard_init
{
public:
	_keyboard_init()
	{
		tcgetattr(0, &initial_settings);
		new_settings = initial_settings;
		new_settings.c_lflag &= ~ICANON;
		new_settings.c_lflag &= ~ECHO;
		new_settings.c_cc[VMIN] = 1;
		new_settings.c_cc[VTIME] = 0;
		tcsetattr(0, TCSANOW, &new_settings);
	}

	~_keyboard_init()
	{
		tcsetattr(0, TCSANOW, &initial_settings);
	}

	int _kbhit()
	{
		unsigned char ch;
		int nread;

		if (peek_character != -1) return 1;
		new_settings.c_cc[VMIN] = 0;
		tcsetattr(0, TCSANOW, &new_settings);
		nread = read(0, &ch, 1);
		new_settings.c_cc[VMIN] = 1;
		tcsetattr(0, TCSANOW, &new_settings);
		if (nread == 1)
		{
			peek_character = ch;
			return 1;
		}
		return 0;
	}

	int _getch()
	{
		char ch;

		if (peek_character != -1)
		{
			ch = peek_character;
			peek_character = -1;
			return ch;
		}
		int nread = read(0, &ch, 1);

		return (nread == 1) ? ch : 0;
	}

protected:
	struct termios initial_settings, new_settings;
	int peek_character = -1;
};


_keyboard_init g_key_;

int _kbhit()
{
	return g_key_._kbhit();
}

int _getch()
{
	return g_key_._getch();
}


#endif