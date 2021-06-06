// g++ `pkg-config --cflags sdl2 SDL2_image SDL2_mixer` sdl.cpp `pkg-config --libs sdl2 SDL2_image SDL2_mixer`; ./a.out

#include <SDL.h>
#include <chrono>
#include <thread>
#include <cstdlib>

#define WIDTH 1280
#define HIDTH 960

class CApp {
private:
	bool Running;
	SDL_Window *screen;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	Uint32 myPixels [WIDTH*HIDTH];
	char conwayData [2][HIDTH][WIDTH];
	char conwayChannel = 0;
public:
	CApp ();
	int OnExecute ();
public:
	bool OnInit ();
	void OnEvent (SDL_Event *Event);
	void OnLoop ();
	void OnRender ();
	void OnCleanup ();
};

bool CApp::OnInit ()
{
	if (SDL_Init (SDL_INIT_EVERYTHING) < 0)
		return false;
	screen = SDL_CreateWindow (
		"hi",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WIDTH, HIDTH, 0);
	renderer = SDL_CreateRenderer (screen, -1, 0);
	SDL_SetRenderDrawColor (renderer, 0,0,0,255);
	SDL_RenderClear (renderer);
	SDL_RenderPresent (renderer);
	
	texture = SDL_CreateTexture (
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		WIDTH, HIDTH);
	for (int i = 0; i < WIDTH * HIDTH; ++i)
		myPixels[i] = 0xff000000;
	for (int c = 0; c < 2; ++c)
		for (int x = 0; x < WIDTH; ++x)
			for (int y = 0; y < HIDTH; ++y)
				conwayData[c][y][x] = rand() & 1;
	conwayChannel = 0;
	return true;
}

void CApp::OnEvent (SDL_Event *Event)
{
	if (Event->type == SDL_QUIT)
		Running = false;
}

void CApp::OnLoop ()
{
	int neighborSum, x, y, i;
	char (&lastData)[HIDTH][WIDTH] = conwayData[conwayChannel];
	conwayChannel = 1 - conwayChannel;
	char (&currData)[HIDTH][WIDTH] = conwayData[conwayChannel];
	for (x = 1; x < WIDTH - 1; ++x)
	{
		for (y = 1; y < HIDTH - 1; ++y)
		{
			neighborSum
				= lastData[y-1][x-1]
				+ lastData[y-1][x  ]
				+ lastData[y-1][x+1]
				+ lastData[y  ][x-1]
				+ lastData[y  ][x+1]
				+ lastData[y+1][x-1]
				+ lastData[y+1][x  ]
				+ lastData[y+1][x+1];
			if (neighborSum == 3)
				conwayData[conwayChannel][y][x] = 1;
			else if (neighborSum == 2)
				currData[y][x] = lastData[y][x];
			else
				currData[y][x] = 0;
		}
	}
	i = 0;
	for (y = 0; y < HIDTH; ++y)
	{
		for (x = 0; x < WIDTH; ++x)
		{
			myPixels[i] = !currData[y][x] ? 0xff000000 : 0xffffffff;
			++i;
		}
	}
}

void CApp::OnRender ()
{
	SDL_UpdateTexture (texture, NULL, myPixels, WIDTH * sizeof (Uint32));
	SDL_RenderClear (renderer);
	SDL_RenderCopy (renderer, texture, NULL, NULL);
	SDL_RenderPresent (renderer);
}

void CApp::OnCleanup ()
{
	SDL_Quit ();
}

CApp::CApp ()
{
	screen = NULL;
	Running = true;
}

int CApp::OnExecute ()
{
	if (OnInit() == false)
		return -1;
	SDL_Event Event;
	while (Running)
	{
		while (SDL_PollEvent (&Event))
		{
			OnEvent (&Event);
		}
		OnLoop ();
		OnRender ();
		std::this_thread::sleep_for (std::chrono::milliseconds (100));
	}
	OnCleanup ();
	return 0;
}

int main (int argc, char **argv)
{
	CApp theApp;
	return theApp.OnExecute();
}
