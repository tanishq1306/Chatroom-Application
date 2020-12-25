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
int uid = 0;

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
	
		++uid;
		thread _thread (handle_client, client_socket, uid);
		lock_guard<mutex> guard (clients_mtx);
		clients.push_back ({uid, string("Anonymous"), client_socket(move(_thread))});
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

// Broadcast message to all clients except the sender
int broadcast_message (string message, int sender_id)
{
	char temp[MAX_LEN];
	strcpy(temp, message.c_str());
	
	for (int i = 0; i < (int)clients.size(); ++i)
	{
		if (clients[i].id != sender_id)
		{
			send (clients[i].socket, temp, sizeof(temp), 0);
		}
	}		
}

// Broadcast a number to all clients except the sender
int broadcast_message (int num, int sender_id)
{
	for (int i = 0; i < (int)clients.size(); ++i)
	{
		if (clients[i].id != sender_id)
		{
			send (clients[i].socket, &num, sizeof(num), 0);
		}
	}		
}

void end_connection (int id)
{
	for (int i = 0; i < (int)clients.size(); ++i)
	{
		if (clients[i].id == id)	
		{
			lock_guard<mutex> guard (clients_mtx);
			clients[i]._thread.detach();
			clients.erase (clients.begin() + i);
			close (clients[i].socket);
			break;
		}
	}				
}


void handle_client (int client_socket, int id)
{
	char name[MAX_LEN], str[MAX_LEN];
	recv (client_socket, name, sizeof(name), 0);
	set_name (id, name);	

	// Display welcome message
	string welcome_message = string(name) + string(" has joined");
	broadcast_message ("#NULL",id);	
	broadcast_message (id,id);								
	broadcast_message (welcome_message,id);	
	shared_print(color(id) + welcome_message + def_col);
	
	while(1)
	{
		int bytes_received = recv (client_socket, str, sizeof(str), 0);
		if (bytes_received <= 0)
			return;
		if (strcmp (str, "#exit") == 0)
		{
			// Display leaving message
			string message = string(name) + string(" has left");		
			broadcast_message ("#NULL",id);			
			broadcast_message (id, id);						
			broadcast_message (message,id);
			shared_print(color(id) + message + def_col);
			end_connection (id);							
			return;
		}
		
		broadcast_message (string(name), id);					
		broadcast_message (id, id);		
		broadcast_message (string(str), id);
		shared_print (color(id) + name + " : " + def_col + str);		
	}	
}
