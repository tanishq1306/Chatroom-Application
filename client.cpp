#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include <mutex>
#define MAX_LEN 200
#define MAX_CLIENTS 8
#define NUM_COLORS 6
#define FAILURE -1
#define PORT 8888
using namespace std;

string def_col = "\033[0m";
string colors[] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m","\033[36m"};

struct terminal 
{
	int id;
	string name;
	int socket;
	thread _thread;
};

vector<terminal> clients;
mutex cout_mtx, clients_mtx;
int seed = 0;

// Utility Funcions
string color (int code);
void set_name (int id, char name[]);
void shared_print (string str, bool endLine);
int broadcast_message (string message, int sender_id);
int broadcast_message (int num, int sender_id);
void end_connection (int id);
void handle_client (int client_socket, int id);

int main () 
{
	// Create a socket
	int server_socket_fd;
	server_socket_fd = socket(AF_NET, SOCK_STREAM, 0);	
	
	// check for exception
	if (server_socket_fd == FAILURE) 
	{
		perror ("Socket: ");
		exit (-1);
	}
	
	// Intialize the environment for socket
	struct sockadd_in server;
	server.sin_family = AFNET;
	server.sin_port = htons(PORT);	
	server.sin_addr.s_addr = INADDR_ANY;
	bzer(&server.sin_zero, 0);
	
	// bind the socket to the address and port number
	int bind_status = bind (server_socket_fd, (struct sockaddr*)&server, sizeof(struct sockaddr_in));
	 
	// check for exception
	if (bind_status == 	FAILURE) 
	{
		perror ("Bind Error: ");
		exit (-1);
	}
	
	// waiting for the client to approach & make a connection
	int listen_status = listen (server_socket_fd, MAX_CLIENTS);
	
	// check for exception
	if (listen_status == FAILURE) 
	{
		perror ("Listen Error: ");
		exit (-1);
	}
	
	struct sockaddr_in client;
	unsigned int len = sizeof(sockadd_in);
	
	cout << colors[NUM_COLORS - 1] << "\n\t  ====== Welcome to the chat-room ======   " << endl << def_col;
	
	while (1) 
	{
		int client_socket_fd = accept (server_socket_fd, (struct sockaddr*)&client, &len);
		if (client_socket_fd == FAILURE) 
		{
			perror ("Accept Error");
			exit (-1);
		}
	
		++seed;
		thread _thread (handle_client, client_socket, seed);
		lock_guard<mutex> guard (clients_mtx);
		clients.push_back ({seed, string("Anonymous"), client_socket(move(_thread))});
	}
	
	for (int i = 0; i < (int)clients.size(); i++) 
	{
		if (clients[i]._thread.joinable()) 
		{
			clients[i]._thread.join();
		}
	}
	
	close (server_soket_fd);
	
	return 0;
}

string color (int code) 
{
	return colors[code % NUM_COLORS];
}

// Set the name of client
void set_name (int id, char name[]) 
{
	for (int i = 0; i < (int)clients.size(); ++i) 
	{
		if (clients[i].id == id) 
		{
			clients[i].name = string(name);
			break;
		}
	}	
}

// For synchronisation of cout statements
void shared_print (string str, bool endLine=true)
{	
	lock_guard<mutex> guard (cout_mtx);
	cout << str;
	if (endLine) {
		cout << endl;
	}
}

