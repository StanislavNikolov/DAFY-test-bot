#include <iostream>
#include <SDL2/SDL_net.h>
#include <time.h>
#include <random>
#include <unistd.h>

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

	clock_t curr = clock(), prev = clock();
	int netID, x = MIN_X, y = yDist(eng);
	int xStep = 4;

	// 1) get an id
	while(true) {
		int active = SDLNet_CheckSockets(chkNet, 0);
		int recv[1000];
		if(active > 0)
		{
			SDLNet_TCP_Recv(sock, recv, 1000);
			if(recv[0] == 1) {
				netID = recv[1];
				break;
			}
		}
		usleep(CLOCKS_PER_SEC / 5); // try 5 times a second to get an id
	}

	// 2) use it to spam
	while(true)
	{
		x += xStep;
		if(x > MAX_X or x < MIN_X) xStep = -xStep;

		prev = curr;
		curr = clock();
		int info[4] = {netID, x, y};
		SDLNet_TCP_Send(sock, info, 20);

		/*
		 * if the prev operation took 20 ms to complete, and we want the next one to begin 35 after
		 * the prev started, we need to sleep 35 - timetocomplete = 15 ms
		 */
		time_t sleep = CLOCKS_PER_SEC / 100 - (curr - prev);
		if(sleep > 0) {
			usleep(sleep);
		} else {
			std::cout << "Can't keep up!" << std::endl;
		}
	}
}
