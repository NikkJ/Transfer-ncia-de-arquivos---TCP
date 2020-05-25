#define _WIN32_WINNT 0x0600
#include <ws2tcpip.h>
#include <Winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>

#include "func_inet.hpp"

#pragma comment(lib, "Ws2_32.lib")

#define PORTA 54000
#define BUF_SIZE 4092

using std::cerr;
using std::endl;
using std::cout;
using std::cin;

int main(int argc, char* argv[]){
	
	WSADATA data;
	if(WSAStartup(MAKEWORD(2,2),&data)!=0){
		cerr<<"ERRO AO INICIAR"<<endl;
		exit(1);
	}
	hostent *h;
	sockaddr_in canal;
	
	/*h = gethostbyname("oblumenauense.com.br");
	if(!h){
		cerr<<"ERRO AO PEGAR IP"<<endl;
		exit(1);
	} */
	
	ZeroMemory(&canal,sizeof(canal));
	canal.sin_family = PF_INET;
	canal.sin_port = htons(PORTA);
	//memcpy(&canal.sin_addr,h->h_addr_list[0],h->h_length);
	inet_pton(AF_INET,"127.0.0.1",&canal.sin_addr);
	SOCKET s = socket(AF_INET,SOCK_STREAM,0);
	
	if(s == INVALID_SOCKET){
		cerr<<"ERRO AO CRIAR SOCKET"<<endl;
		exit(1);
	}
	
	cout<<"Tentando se conectar ao servidor na porta "<<PORTA<<endl;
	int c = connect(s,(sockaddr*)&canal,sizeof(canal));
	
	if(c == SOCKET_ERROR){
		cerr<<"Erro ao se conectar"<<endl;
		exit(1);
	}
	cout<<"Conectado"<<endl;
	char buffer[BUF_SIZE];
	char filename[100];
	
	while(1){
		cout<<">";
		cin>>filename;
		if(strlen(filename)==0){
			exit(1);
		}
		int enviado = send(s, filename,BUF_SIZE,0);
		
		cout<<"Pedindo arquivo: "<< filename<<endl;
		//Sleep(5000);
		if(enviado != SOCKET_ERROR){
			
			ZeroMemory(&buffer,BUF_SIZE);
			
			int bytes = recv(s,buffer,BUF_SIZE,0);
			
			//cout<<bytes<<endl;
			if(strcmp(buffer,"0")==0){
				cerr<<"Erro ao abrir o arquivo no server"<<endl;
			}else if(bytes>1){
				FILE *fp = fopen(filename,"w+");
				if(fp!=nullptr){
					cout<<"Escrevendo no arquivo"<<endl;
					fwrite(buffer,1,bytes,fp);
					fclose(fp);
				}
				cout<<"Arquivo recuperado!"<<endl;
			}else{
				cout<<"Arquivo inexistente ou sem tamanho"<<endl;
			}
		}else{
			cout<<"Nada enviado	"<<endl;
		}
	}
	cout<<"Encerrando conexao"<<endl;
	closesocket(s);
	return 0;
}
