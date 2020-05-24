#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <deque>

using namespace std;
using std::ifstream;

void MainLoop();
void CommandMode();
void ProcessAnalogSticks();
bool LoadSamples(int);

unsigned int timing = 0;
bool doubleHat = false;
int lastHit = 0, currentHit = 0;
int baseTicks = 0;
bool hatPlayed = false;

const int jdz = 32000;
const int minv = -32768;
const int maxv = 32767;
bool pedalpressed = false;
bool ohh_playing = false;
bool double_bass = false;
bool doublevelsnare = true;
bool quit = false;
bool commandmode = false;
bool lastcommandisvelocity = false;
bool dual = false;

int volume = 3;

bool lcu = false;
bool lcd = false;
bool lcl = false;
bool lcr = false;
bool rcu = false;
bool rcd = false;
bool rcl = false;
bool rcr = false;

bool lpu = false;
bool lpd = false;
bool lpl = false;
bool lpr = false;
bool rpu = false;
bool rpd = false;
bool rpl = false;
bool rpr = false;

SDL_Joystick *joy;
SDL_Joystick *joy2;

std::deque<string> drumkits;
std::deque<string> drumkitnames;
int currentdrumkit = 0;
int drumkit_count = 0;

Mix_Chunk   *kick[5],
*snare[5],
*rimshot[5],
*sidestick[5],
*ohh[5],
*chh[5],
*pedalclose[5],
*cymbal1[5],
*cymbal2[5],
*cymbal3[5],
*ride[5],
*rideedge[5],
*hi[5],
*mid[5],
*low[5],
*fl[5],
*china[5],
*last = NULL;

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;

bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
	{
	    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Init Error", SDL_GetError(), NULL);
		return false;
	} else {
		cout << "Done.\n";
	}
	cout << "Initializing SDL audio mixer... ";
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0)
	{
		cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
		return false;
	}
	else cout << "Done.\n";
	cout << "Scanning for game controllers... ";
	if (SDL_NumJoysticks() < 1)
	{
		cout << "WARNING: Game controller not found!\n";
	}
	else if (SDL_NumJoysticks() == 1)
	{
		cout << "One controller found.\n";
		cout << "\nConnecting to game controller... ";
		joy = SDL_JoystickOpen(0);
		if (joy == NULL)
		{
			cout << "\nERROR: Unable to connect to game controller! SDL Error: " << SDL_GetError() << endl;
			return false;
		}
		else cout << "Done.\n";
	}
	else
	{
		cout << SDL_NumJoysticks() << " controllers found.\n";
		cout << "\nConnecting to game controller... ";
		joy = SDL_JoystickOpen(0);
		joy2 = SDL_JoystickOpen(1);
		if (joy == NULL || joy2 == NULL)
		{
			cout << "\nERROR: Unable to connect to game controller! SDL Error: " << SDL_GetError() << endl;
			return false;
		}
		else
		{
			cout << "Done.\n";
			cout << "If you get unepected results, close the program,\nplug the main controller before plugging the foot controller, then\ntry again.\n";
		}
		dual = true;
	}
	if (SDL_NumJoysticks() > 2)
	{
		cout << "More than two joystick detected.\nGamepadDrums will try to ignore the other joyticks, but if you\nget unexpected results, remove the other joyticks\nthen restart the program.\n";
	}

	Mix_AllocateChannels(128);

	return true;
}

bool loadMedia()
{
	bool success = true;
	string filename, name;

	cout << "Getting drum kit list...\n";
	ifstream drumkitlist("drumkit_list.txt", ios::in);
	if (!drumkitlist)
	{
		cout << "Error: Drum kit list not found!\n";
		return false;
	}

	drumkitlist >> drumkit_count;
	cout << drumkit_count << " drum kits are listed.\n";

	for (int i = 0; i < drumkit_count; i++)
	{
		drumkitlist >> filename;
		drumkitlist >> name;
		drumkits.push_back(filename);
		drumkitnames.push_back(name);
	}

	success = LoadSamples(currentdrumkit);

	return success;
}

bool LoadSamples(int index)
{
	ifstream samplelist(drumkits[index].c_str(), ios::in);
	string filename;

	cout << "\nLoading " << drumkitnames[index] << " drum kit... ";

	if (!samplelist)
	{
		cout << "\nError: Drum kit sample list not found!\n";
		return false;
	}

	for (int i = 0; i < 5; i++)
	{
		Mix_FreeChunk(kick[i]);
		Mix_FreeChunk(snare[i]);
		Mix_FreeChunk(rimshot[i]);
		Mix_FreeChunk(sidestick[i]);
		Mix_FreeChunk(ohh[i]);
		Mix_FreeChunk(chh[i]);
		Mix_FreeChunk(pedalclose[i]);
		Mix_FreeChunk(cymbal1[i]);
		Mix_FreeChunk(cymbal2[i]);
		Mix_FreeChunk(cymbal3[i]);
		Mix_FreeChunk(ride[i]);
		Mix_FreeChunk(rideedge[i]);
		Mix_FreeChunk(china[i]);
		Mix_FreeChunk(hi[i]);
		Mix_FreeChunk(mid[i]);
		Mix_FreeChunk(low[i]);
		Mix_FreeChunk(fl[i]);
	}

	for (int i = 0; i < 5; i++)
	{
		kick[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			kick[i] = Mix_LoadWAV(filename.c_str());
		if (kick[i] == NULL && filename != "empty")
			cout << "Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		snare[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			snare[i] = Mix_LoadWAV(filename.c_str());
		if (snare[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		rimshot[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			rimshot[i] = Mix_LoadWAV(filename.c_str());
		if (rimshot[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		sidestick[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			sidestick[i] = Mix_LoadWAV(filename.c_str());
		if (sidestick[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		chh[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			chh[i] = Mix_LoadWAV(filename.c_str());
		if (chh[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		ohh[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			ohh[i] = Mix_LoadWAV(filename.c_str());
		if (ohh[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		pedalclose[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			pedalclose[i] = Mix_LoadWAV(filename.c_str());
		if (pedalclose[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		cymbal1[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			cymbal1[i] = Mix_LoadWAV(filename.c_str());
		if (cymbal1[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		cymbal2[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			cymbal2[i] = Mix_LoadWAV(filename.c_str());
		if (cymbal2[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		cymbal3[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			cymbal3[i] = Mix_LoadWAV(filename.c_str());
		if (cymbal3[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		ride[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			ride[i] = Mix_LoadWAV(filename.c_str());
		if (ride[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		rideedge[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			rideedge[i] = Mix_LoadWAV(filename.c_str());
		if (rideedge[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		china[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			china[i] = Mix_LoadWAV(filename.c_str());
		if (china[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		hi[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			hi[i] = Mix_LoadWAV(filename.c_str());
		if (hi[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		mid[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			mid[i] = Mix_LoadWAV(filename.c_str());
		if (mid[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		low[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			low[i] = Mix_LoadWAV(filename.c_str());
		if (low[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}
	for (int i = 0; i < 5; i++)
	{
		fl[i] = NULL;
		samplelist >> filename;
		if (filename != "empty")
			fl[i] = Mix_LoadWAV(filename.c_str());
		if (fl[i] == NULL && filename != "empty")
			cout << "Error: Failed to load \"" << filename << "\", " << Mix_GetError() << endl;
	}

	cout << "Done.\n";
	return true;
}

void close()
{
	cout << "Unloading drum samples... ";
	for (int i = 0; i < 5; i++)
	{
		Mix_FreeChunk(kick[i]);
		Mix_FreeChunk(snare[i]);
		Mix_FreeChunk(rimshot[i]);
		Mix_FreeChunk(sidestick[i]);
		Mix_FreeChunk(ohh[i]);
		Mix_FreeChunk(chh[i]);
		Mix_FreeChunk(pedalclose[i]);
		Mix_FreeChunk(cymbal1[i]);
		Mix_FreeChunk(cymbal2[i]);
		Mix_FreeChunk(cymbal3[i]);
		Mix_FreeChunk(ride[i]);
		Mix_FreeChunk(rideedge[i]);
		Mix_FreeChunk(china[i]);
		Mix_FreeChunk(hi[i]);
		Mix_FreeChunk(mid[i]);
		Mix_FreeChunk(low[i]);
		Mix_FreeChunk(fl[i]);
	}
	cout << "Done.\n";

	cout << "Releasing joystick... ";
	SDL_JoystickClose(joy);
	joy = NULL;
	cout << "Done.\n";

	cout << "Quitting SDL... ";
	Mix_Quit();
	SDL_DestroyWindow( window );
	SDL_Quit();
	cout << "Done.";
}

int main(int argc, char* args[])
{
	if (!init())
	{
		cout << "Failed to initialize!\n";
	}
	else
	{
		if (!loadMedia())
		{
			cin.get();
		}
		else
		{
			MainLoop();
		}
	}

	close();


	return 0;
}

void MainLoop()
{
	SDL_Event e;
	int v;

	cout << "\nGamepadDrums 4.0\nKyle Alexander Buan - Feb 12 2016\n\nReady to accept controller input.\n\n";

	while (!quit)
	{
		if (doubleHat)
		{
			if (currentHit > 0 && lastHit > 0 && !hatPlayed)
				if ((SDL_GetTicks() - baseTicks) > ((currentHit - lastHit) / 2))
				{
					if (!ohh_playing)
						Mix_PlayChannel(63, chh[volume - 1<0 ? 0 : volume - 1], 0);
					hatPlayed = true;
				}
		}
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_JOYHATMOTION)
			{
				if (e.jhat.value == SDL_HAT_LEFT)
				{
					if (ohh_playing)
					{
						Mix_HaltChannel(63);
						Mix_PlayChannel(63, pedalclose[volume], 0);
						ohh_playing = false;
					}
					else
					{
						Mix_PlayChannel(63, chh[volume], 0);
						lastHit = currentHit;
						currentHit = baseTicks = SDL_GetTicks();
						hatPlayed = false;
					}
				}
				else if (e.jhat.value == SDL_HAT_RIGHT)
				{
					if (last != NULL)
						Mix_PlayChannel(-1, last, 0);
				}
				if (e.jhat.value == SDL_HAT_UP)
				{
					Mix_PlayChannel(-1, ride[volume], 0);
				}
				else if (e.jhat.value == SDL_HAT_DOWN)
				{
					ohh_playing = true;
					if (dual)
					{
						if (pedalpressed)
							Mix_PlayChannel(63, chh[volume], 0);
						else
							Mix_PlayChannel(63, ohh[volume], 0);
					}
					else
						Mix_PlayChannel(63, ohh[volume], 0);
				}
			}
			else if (e.type == SDL_JOYBUTTONDOWN)
			{
				if (e.jdevice.which == 0)
				{
					if (e.jbutton.button == 0)
					{
						Mix_PlayChannel(63, low[volume], 0);
						last = low[volume];
					}
					else if (e.jbutton.button == 1)
					{
						Mix_PlayChannel(-1, mid[volume], 0);
						last = mid[volume];
					}
					else if (e.jbutton.button == 2)
					{
						if (doublevelsnare)
						{
							Mix_PlayChannel(-1, snare[volume - 1 < 0 ? 0 : volume - 1], 0);
							last = snare[volume - 1 < 0 ? 0 : volume - 1];
						}
						else
						{
							Mix_PlayChannel(-1, hi[volume], 0);
							last = hi[volume];
						}
					}
					else if (e.jbutton.button == 3)
					{
						Mix_PlayChannel(-1, snare[volume], 0);
						last = snare[volume];
					}
					else if (e.jbutton.button == 6)
					{
						Mix_PlayChannel(-1, cymbal1[volume], 0);
					}
					else if (e.jbutton.button == 7)
					{
						if (dual)
							Mix_PlayChannel(-1, china[volume], 0);
						else
						{
							Mix_PlayChannel(-1, kick[volume], 0);
							if (double_bass) last = kick[volume];
						}
					}
					else if (e.jbutton.button == 4)
					{
						Mix_PlayChannel(-1, cymbal2[volume], 0);
					}
					else if (e.jbutton.button == 5)
					{
						Mix_PlayChannel(-1, fl[volume], 0);
					}
					else if (e.jbutton.button == 11)
					{
						Mix_PlayChannel(-1, rideedge[volume], 0);
					}
					else if (e.jbutton.button == 10)
					{
						cout << "\nWaiting for second keypress... ";
						lastcommandisvelocity = false;
						CommandMode();
					}
				}
				else if (e.jdevice.which == 1)
				{
					if (e.jbutton.button == 4 || e.jbutton.button == 6)
					{
						Mix_HaltChannel(63);
						Mix_PlayChannel(63, pedalclose[volume], 0);
						pedalpressed = true;
					}
					if (e.jbutton.button == 1 || e.jbutton.button == 5 || e.jbutton.button == 2 || e.jbutton.button == 7)
					{
						Mix_PlayChannel(-1, kick[volume], 0);
					}
				}
			}
			else if (e.type == SDL_JOYBUTTONUP)
			{
				if (e.jdevice.which == 1)
				{
					if (e.jbutton.button == 4 || e.jbutton.button == 6)
					{
						pedalpressed = false;
					}
				}
			}
			else if (e.type == SDL_JOYAXISMOTION)
			{
				if (e.jaxis.axis == 0)
				{
					// left-right
					if (e.jaxis.value <= minv)
					{
						lpl = lcl;
						lcl = true;
					}
					else if (e.jaxis.value >= maxv)
					{
						lpr = lcr;
						lcr = true;
					}
					else
					{
						lcr = false;
						lcr = false;
					}
				}
				else if (e.jaxis.axis == 1)
				{
					// up-down
					if ((int)e.jaxis.value <= minv)
					{
						lpu = lcu;
						lcu = true;
					}
					else if (e.jaxis.value >= maxv)
					{
						lpd = lcd;
						lcd = true;
					}
					else
					{
						lcd = false;
						lcu = false;
					}
				}
				else if (e.jaxis.axis == 2 || e.jaxis.axis == 3)
				{
					// right left-right
					if (e.jaxis.value <= minv)
					{
						rpl = rcl;
						rcl = true;
					}
					else if (e.jaxis.value >= maxv)
					{
						rpr = rcr;
						rcr = true;
					}
					else
					{
						rcr = false;
						rcl = false;
					}
				}
				else if (e.jaxis.axis == 4)
				{
					// right up-down
					if (e.jaxis.value <= minv)
					{
						rpu = rcu;
						rcu = true;
					}
					else if (e.jaxis.value >= maxv)
					{
						rpd = rcd;
						rcd = true;
					}
					else
					{
						rcu = false;
						rcd = false;
					}
				}
			}
			ProcessAnalogSticks();
		}
	}
}

void CommandMode()
{
	SDL_Event e;
	bool cont = true;

	while (SDL_PollEvent(&e) != 0 || cont)
	{
		if (e.type == SDL_QUIT)
		{
			quit = true;
		}

		else if (e.type == SDL_JOYBUTTONDOWN)
		{

			if (e.jbutton.button == 0)
			{
				doubleHat = !doubleHat;
				if (doubleHat)
					cout << "Double closed hi-hat funcion turned on.\n";
				else
					cout << "Double closed hi-hat function turned off.\n";
				lastcommandisvelocity = false;
			}
			else if (e.jbutton.button == 1)
			{
				double_bass = !double_bass;
				if (double_bass)
					cout << "Double bass activated.\n";
				else
					cout << "Double bass deactivated.\n";
				lastcommandisvelocity = false;
			}
			else if (e.jbutton.button == 2)
			{
				doublevelsnare = !doublevelsnare;
				if (doublevelsnare)
					cout << "Multi-velocity snare activated.\n";
				else
					cout << "Multi-velocity snare deactivated.\n";
			}
			else if (e.jbutton.button == 10)
			{
				quit = true;
			}
			else cout << "Unknown command button: " << (int)e.jbutton.button << endl;
			cont = false;
			lastcommandisvelocity = false;
		}
	}


}

void ProcessAnalogSticks()
{
	if (lcd && !lpd && volume > 0)
	{
		volume--;
		if (lastcommandisvelocity)
			cout << "\rVelocity: " << volume + 1;
		else
			cout << "Velocity: " << volume + 1;
		lastcommandisvelocity = true;
	}
	else if (lcu && !lpu && volume < 4)
	{
		volume++;
		if (lastcommandisvelocity)
			cout << "\rVelocity: " << volume + 1;
		else
			cout << "Velocity: " << volume + 1;
		lastcommandisvelocity = true;
	}

	if (lcl && !lpl && currentdrumkit > 0)
	{
		//cout << "left" << endl;
		currentdrumkit--;
		LoadSamples(currentdrumkit);
	}
	else if (lcr && !lpr && currentdrumkit < (drumkit_count - 1))
	{
		//cout << "right" << endl;
		currentdrumkit++;
		LoadSamples(currentdrumkit);
	}


	if (rcu && !rpu)
	{
		Mix_PlayChannel(-1, cymbal3[volume], 0);
	}
	else if (rcd && !rpd)
	{
		Mix_PlayChannel(-1, china[volume], 0);
	}

	if (rcl && !rpl)
	{
		Mix_PlayChannel(-1, rimshot[volume], 0);
	}
	else if (rcr && !rpr)
	{
		Mix_PlayChannel(-1, sidestick[volume - 1<0 ? 0 : volume - 1], 0);
	}

	lcu = false;
	lcd = false;
	lcl = false;
	lcr = false;
	rcu = false;
	rcd = false;
	rcl = false;
	rcr = false;

}
