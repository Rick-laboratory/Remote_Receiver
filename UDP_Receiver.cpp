#include "UDP_Receiver.h"

int UDP_Receiver::UDP_Receiver_INIT()
{
	if (int errorcode = WSAStartup(MAKEWORD(2, 2), &WSA)){
		wprintf(L"WSAStartup function failed with error = %d\n", errorcode);
		return EXIT_FAILURE;
	}

	Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (Socket == INVALID_SOCKET) {
		wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
		return EXIT_FAILURE;
	}

	if (setsockopt(Socket, SOL_SOCKET, SO_BROADCAST, &Broadcast, sizeof(Broadcast)) == SOCKET_ERROR){
		wprintf(L"setsockopt function failed with error = %d\n", WSAGetLastError());
		closesocket(Socket);
		return EXIT_FAILURE;
	}

	Addr_recv.sin_family = AF_INET;
	Addr_recv.sin_port = htons(PORT);
	Addr_recv.sin_addr.s_addr = INADDR_ANY;
	return 0;
}

int UDP_Receiver::UDP_Receiver_BIND()
{
	if (bind(Socket, (sockaddr*)&Addr_recv, sizeof(Addr_recv)) == SOCKET_ERROR) {
		wprintf(L"bind function failed with error = %d\n", WSAGetLastError());
		closesocket(Socket);
	}
	return 0;
}

int UDP_Receiver::UDP_Receiver_EXEC()
{
	while (true)
	{
		recv(Socket, Buffer_recv, Buffer_len, 0); 
		// TODO: Implement AES 256 / Invalid instructions should be filtered automaticaly   			//Clap One
		// due to the likelyhood of invalid initializations in a case of decryption of provoking broadcasts. 
		// Three flies in two claps basicly.
		// TODO: Handle Invalid initialisation.									//Clap Two
		// Extra security layer could be to only initialise instructions from authorised IDs :) 
		// It will also avoid colisions between mutliple servers in one network
		sscanf_s(Buffer_recv, "%d %d %d", &instr[0], &instr[1], &instr[2]);
		(*MI_Obj.*func_ptr[instr[0]])(pt);
	}
	return 0;
}
