#include <iostream>
#include <SDL2/SDL_net.h>
#include <time.h>
#include <random>

const int MIN_X = 40, MAX_X = 400;

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_int_distribution<int> yDist(0, 300);

int main(int argc, char** argv)
{
	SDLNet_Init();
	TCPsocket sock;
	SDLNet_SocketSet chkNet = SDLNet_AllocSocketSet(1);
	IPaddress ip;

	SDLNet_ResolveHost(&ip, argv[1], 1234);
	sock = SDLNet_TCP_Open(&ip);
	if(!sock) {
		std::cout << "Failed to connect" << std::endl;
		return 0;
	} else {
		SDLNet_TCP_AddSocket(chkNet, sock);
	}

	clock_t lastSend = clock();
	int netID, x = MIN_X, y = yDist(eng);
	int xStep = 4;

	while(true)
	{
		if(lastSend + CLOCKS_PER_SEC / 30 < clock())
		{
			x += xStep;
			if(x > MAX_X or x < MIN_X) xStep = -xStep;

			lastSend = clock();
			int info[4] = {netID, x, y};
			SDLNet_TCP_Send(sock, info, 20);
		}

		int active = SDLNet_CheckSockets(chkNet, 0);
		int recv[1000];
		if(active > 0)
		{
			SDLNet_TCP_Recv(sock, recv, 1000);
			netID = recv[0];
		}
	}
}
