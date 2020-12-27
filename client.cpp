#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <signal.h>
#include <mutex>
#define MAX_LEN 200
#define NUM_COLORS 6
#define PORT 8888
#define FAILURE -1

using namespace std;

bool exit_flag = false;
thread t_send, t_recv;
int client_socket_fd;
string def_col="\033[0m";
string colors[]={"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};

void catch_ctrl_c (int signal);
string color (int code);
void eraseText (int cnt);
void send_message (int client_socket_fd);
void recv_message (int client_socket_fd);

int main() 
{
	client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket_fd == FAILURE) 
	{
		perror("Socket: ");
		exit(-1);
	}
	
	struct sockaddr_in client;
	client.sin_family = AF_INET;
	client.sin_port = htons(PORT); // Port no. of server
	client.sin_addr.s_addr = INADDR_ANY;
	bzero(&client.sin_zero,0);

	int connect_status = connect(client_socket_fd, (struct sockaddr *)&client, 
									sizeof(struct sockaddr_in));
	
	if (connect_status == FAILURE) 
	{
		perror("Connect: ");
		exit(-1);
	}

	signal (SIGINT, catch_ctrl_c);
	char name[MAX_LEN];
	cout << "Enter Your Name: ";
	cin.getline(name, MAX_LEN);
	
	send (client_socket_fd, name, sizeof(name), 0);
	
	cout << colors[NUM_COLORS - 1] << "\n\t  ====== Welcome to the chat-room ======   " << endl << def_col;
	
	thread t1 (send_message, client_socket_fd);
	thread t2 (recv_message, client_socket_fd);
	
	t_send = move(t1);
	t_recv = move(t2);
	
	if (t_send.joinable()) 
	{
		t_send.join();
	}
	
	if (t_recv.joinable()) 
	{
		t_recv.join();
	}
	
	return 0;
}

// Handler for "Ctrl + C"
void catch_ctrl_c (int signal) 
{
	char str[MAX_LEN] = "#exit";
	send (client_socket_fd, str, sizeof(str), 0);
	exit_flag = true;
	t_send.detach();
	t_recv.detach();
	close(client_socket_fd);
	exit(signal);
}

string color(int code)
{
	return colors[code%NUM_COLORS];
}

// Erase text from terminal
void eraseText(int cnt)
{
	char back_space = 8;
	for (int i = 0; i < cnt; i++)
	{
		cout << back_space;
	}	
}

// Send message to everyone
void send_message (int client_socket_fd)
{
	while (1)
	{
		cout << colors[1] << "You : " << def_col;
		char str[MAX_LEN];
		cin.getline(str, MAX_LEN);
		
		send (client_socket_fd, str, sizeof(str), 0);
		if (strcmp(str, "#exit") == 0)
		{
			exit_flag = true;
			t_recv.detach();	
			close (client_socket_fd);
			return;
		}	
	}		
}

// Receive message
void recv_message (int client_socket_fd)
{
	while (1)
	{
		if (exit_flag)
		{
			return;
		}
		
		char name[MAX_LEN], str[MAX_LEN];
		int color_code;
		
		int bytes_received = recv(client_socket_fd, name, sizeof(name), 0);
		if(bytes_received <= 0)
		{
			continue;
		}
		
		recv (client_socket_fd, &color_code, sizeof(color_code), 0);
		recv (client_socket_fd, str, sizeof(str), 0);
		eraseText (6);
		
		if (strcmp(name, "#NULL") != 0) 
		{
			cout << color(color_code) << name << " : " << def_col << str << endl;
		}
		else
		{
			cout << color(color_code) << str << endl;
		}
		
		cout << colors[1] << "You : " << def_col;
		fflush(stdout);
	}	
}
