#include <stdio.h>
#if defined(__linux__)
#include <unistd.h>
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

		// check) aleady exist
		if (peek_character != -1 && peek_character != 27) 
			return 1;

		// 1) set nonblock
		new_settings.c_cc[VMIN] = 0;
		tcsetattr(0, TCSANOW, &new_settings);

		if(peek_character == -1)
		{
			// 2) get first character
			nread = read(0, &ch, 1);

			// 3) if not exist
			if (nread == 0)
			{
				// - restore 
				new_settings.c_cc[VMIN] = 1;
				tcsetattr(0, TCSANOW, &new_settings);

				// return)
				return 0;
			}

			// 4) set character
			peek_character = ch;

			// 4) if not ESCAPE character
			if(ch != 27)
			{
				// - restore 
				new_settings.c_cc[VMIN] = 1;
				tcsetattr(0, TCSANOW, &new_settings);

				// return)
				return 1;
			}
		}

		// 5) get second character
		nread = read(0, &ch, 1);

		// 6) not exist
		if(nread == 0)
		{
			// - restore 
			new_settings.c_cc[VMIN] = 1;
			tcsetattr(0, TCSANOW, &new_settings);

			// return)
			return 1;
		}

		// 7) 
		if (ch != 91 && ch != 79)
		{
			// - set
			peek_character_sub = ch;

			// - restore 
			new_settings.c_cc[VMIN] = 1;
			tcsetattr(0, TCSANOW, &new_settings);

			// return)
			return 1;
		}

		// 8) 
		peek_character = (peek_character << 8) | ch;
		
		// 9) get thrid character
		nread = read(0, &ch, 1);

		// 10) set 
		peek_character = (peek_character << 8) | ch;

		// - restore 
		new_settings.c_cc[VMIN] = 1;
		tcsetattr(0, TCSANOW, &new_settings);

		// return)
		return 1;
	}

	int _getch()
	{
		char ch;

		if (peek_character != -1)
		{
			ch = peek_character;
			peek_character = peek_character_sub;
			peek_character_sub = -1;
			return ch;
		}
		int nread = read(0, &ch, 1);

		return (nread == 1) ? ch : 0;
	}

protected:
	struct termios initial_settings, new_settings;
	int peek_character = -1;
	int peek_character_sub = -1;
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