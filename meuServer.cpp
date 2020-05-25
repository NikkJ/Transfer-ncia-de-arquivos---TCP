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
	
	sockaddr_in server;
	ZeroMemory(&server,sizeof(server));
	server.sin_addr.S_un.S_addr = INADDR_ANY;
	server.sin_port = htons(PORTA);
	server.sin_family = AF_INET;
	
	SOCKET s;
	
	if((s = socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET){
		cerr<<"Erro ao criar socket"<<endl;
		exit(1);
	}
	
	cout<<"Conectando na porta "<<PORTA<<"..."<<endl;
	int b;
	if((b = bind(s,(sockaddr*)&server,sizeof(server)))==SOCKET_ERROR){
		cerr<<"Erro ao ligar a porta "<<PORTA<<endl;
		closesocket(s);
		exit(1);
	}
	
	cout<<"Esperando conexao na porta "<<PORTA<<endl;
	int l;
	if((l = listen(s,SOMAXCONN))==SOCKET_ERROR){
		cerr<<"Erro ao ouvir na porta "<<PORTA<<endl;
		closesocket(s);
		exit(1);
	}
	sockaddr_in clienteAddr;
	int clAddrLen = sizeof(clienteAddr);
	
	ZeroMemory(&clienteAddr,clAddrLen);
	
		
	SOCKET client = accept(s,(sockaddr*)&clienteAddr,&clAddrLen);
	

	
	if(client == INVALID_SOCKET){
		cerr<<"Erro ao criar conexao com o cliente"<<endl;
		closesocket(s);
		exit(1);
	}
	
		
	char recClient[BUF_SIZE];
	FILE *fp;
	int tamLido;
	cout<<"Cliente conectado"<<endl;
	while(1){
		cout<<"Pronto"<<endl;
		//Sleep(5000);
		int bytes = recv(client,recClient,BUF_SIZE,0);
		cout<<bytes<<endl;
		if(bytes != SOCKET_ERROR){
			
			if((fp = fopen(recClient,"r"))== nullptr){
				cerr<<"Erro ao abrir arquivo"<<endl;
				send(client,"0",strlen("0"),0);
			}else if(GetFileAttributes(recClient) != INVALID_FILE_ATTRIBUTES){//verificar se existe arquivo no diretorio
				cout<<"Buscando arquivo: "<<recClient<<endl;
				fseek(fp,0,SEEK_END);
				long arqTam = ftell(fp);
				char *dadosArquivo = new char[arqTam];
				rewind(fp);
				
				if((tamLido = fread(dadosArquivo,1,arqTam,fp))!=arqTam){
					cerr<<"Erro na leitura do arquivo"<<endl;
					fclose(fp);
				}
				
				int enviando = send(client,dadosArquivo,tamLido,0);
				
				if(enviando == SOCKET_ERROR){
					cerr<<"Erro ao enviar o arquivo"<<endl;
				}else{
					cout<<"Arquivo enviado"<<endl;
				}
			}else{
				cerr<<"Erro ao abrir arquivo"<<endl;
				send(client,"0",strlen("0"),0);
			}
			fclose(fp);
		}
	}
	closesocket(client);
	WSACleanup();
	return 0;	
}

