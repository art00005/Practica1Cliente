/*******************************************************
Protocolos de Transporte
Grado en Ingeniería Telemática
Dpto. Ingeníería de Telecomunicación
Univerisdad de Jaén

Fichero: cliente.c
Versión: 1.0
Fecha: 23/09/2012
Descripción:
	Cliente de eco sencillo TCP.

Autor: Juan Carlos Cuevas Martínez
Alumnos Jesus Bermejo Torrent - Andrés del Rio Tassara

*******************************************************/
#include <stdio.h>
#include <winsock.h>
#include <time.h>
#include <conio.h>

#include "protocol.h"





int main(int *argc, char *argv[])
{
	SOCKET sockfd;
	struct sockaddr_in server_in;
	char buffer_in[1024], buffer_out[1024],input[1024];
	int recibidos=0,enviados=0;
	int estado=S_HELO;
	char option;

	int NUM1=0;												//_________________________introducimos dos variables NUM1 & NUM2______________________
	int NUM2=0;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

    char ipdest[16];
	char default_ip[16]="127.0.0.1";
	
	//Inicialización Windows sockets
	wVersionRequested=MAKEWORD(1,1);
	err=WSAStartup(wVersionRequested,&wsaData);
	if(err!=0)
		return(0);

	if(LOBYTE(wsaData.wVersion)!=1||HIBYTE(wsaData.wVersion)!=1)
	{
		WSACleanup();
		return(0);
	}
	//Fin: Inicialización Windows sockets

	do{
		sockfd=socket(AF_INET,SOCK_STREAM,0);							//-----------------------Primitiva SOCKET= crea un socket y devuelve un descriptor, en caso contrario devuelve -1 ----------------

		if(sockfd==INVALID_SOCKET)
		{
			printf("CLIENTE> ERROR AL CREAR SOCKET\r\n");
			exit(-1);
		}
		else
		{
			printf("CLIENTE> SOCKET CREADO CORRECTAMENTE\r\n");

		
			printf("CLIENTE> Introduzca la IP destino (pulsar enter para IP por defecto): ");
			gets(ipdest);

			if(strcmp(ipdest,"")==0)
				strcpy(ipdest,default_ip);


			server_in.sin_family=AF_INET;
			server_in.sin_port=htons(TCP_SERVICE_PORT);
			server_in.sin_addr.s_addr=inet_addr(ipdest);
			
			estado=S_HELO;
		
			// establece la conexion de transporte
			if(connect(sockfd,(struct sockaddr*)&server_in,sizeof(server_in))==0)					//--------------------------Primitiva CONNECT= inicia conexión con conector remoto-------------------
			{
				printf("CLIENTE> CONEXION ESTABLECIDA CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			
		
				//Inicio de la máquina de estados
				do{
					switch(estado)
					{
					case S_HELO:
						// Se recibe el mensaje de bienvenida
						break;
					case S_USER:
						// establece la conexion de aplicacion 
						printf("CLIENTE> Introduzca el usuario (enter para salir): ");
						gets(input);
						if(strlen(input)==0)
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);					//_________________sprintf_s: Envía QUIT al buffer_____________
							estado=S_QUIT;
						}
						else

						sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",SC,input,CRLF);			//________________sprintf_s: Envía el usuario al buffer_____________
						break;
					case S_PASS:
						printf("CLIENTE> Introduzca la clave (enter para salir): ");
						gets(input);
						if(strlen(input)==0)
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);					//_________________sprintf_s: Envía QUIT al buffer_____________
							estado=S_QUIT;
						}
						else
							sprintf_s (buffer_out, sizeof(buffer_out), "%s %s%s",PW,input,CRLF);			//_________________sprintf_s: Envía password al buffer_____________
						break;

						//--------------------------------------------EJERCICIO CASO SUM-----------------------------------------------------
					case S_SUM:
						printf("Introduce dos numeros \n");

						do{
							printf("Numero 1 de cuatro digitos: ");
							scanf ("%i",&NUM1);
							fflush(stdin);
						}while (NUM1 > 9999 || NUM1 < -9999);
						
						do{
							printf("Numero 2 de cuatro digitos: ");
							scanf ("%i",&NUM2);
							fflush(stdin);
						}while (NUM2 > 9999 || NUM2 < -9999);
						sprintf_s (buffer_out, sizeof(buffer_out), "%s%s%i%s%i%s",SUM,SP,NUM1,SP,NUM2,CRLF);
						fflush(stdin);															//________________________Limpiamos el buffer despues de usar scanf, ya que nos daba error al usar el siguiente gets___________
						break;
						//---------------------------------------------------------FIN-----------------------------------------------------------------------------
					case S_DATA:
						printf("CLIENTE> Introduzca datos (enter o QUIT para salir): ");
						gets(input);
						if(strlen(input)==0)
						{
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",SD,CRLF);					//_________________sprintf_s: Envía QUIT al buffer_____________
							estado=S_QUIT;
						}
						else
							sprintf_s (buffer_out, sizeof(buffer_out), "%s%s",input,CRLF);				//_________________sprintf_s: Envía DATOS al buffer_____________
						break;
				 
				
					}
					//Envio
					if(estado!=S_HELO){
					// Ejercicio: Comprobar el estado de envio
						enviados=send(sockfd,buffer_out,(int)strlen(buffer_out),0);						//-------------------------Primitiva SEND= envía mensaje -----------------------------
					

					//------------------------------EJERCICIO---------------------------------------------------------------
					
						if (enviados == SOCKET_ERROR){
							printf("Error en el envio");
							estado = S_QUIT;
						}
						if (enviados > 0){
							printf("Se han enviado %d bytes \r\n ", enviados);

						}
						if (enviados == 0){
							printf("Conexion ha sido liberada de forma acordada");
							estado = S_QUIT;
						}
					}

				//------------------------------------FIN-------------------------------------------------------------------

					//Recibo
					recibidos=recv(sockfd,buffer_in,512,0);												//-------------------------Primitiva RECV= recibir mensaje ---------------------------

					if(recibidos<=0)																	//________________________Control de errores de recibidos______________________________
					{
						DWORD error=GetLastError();
						if(recibidos<0)																	//________________________ recibidos=-1 la operación ha fallado___________________________
						{
							printf("CLIENTE> Error %d en la recepción de datos\r\n",error);
							estado=S_QUIT;
						}
						else																			//_________________________recibidos = 0 la conexion se ha liberado de forma acordada______________
						{
							printf("CLIENTE> Conexión con el servidor cerrada\r\n");
							estado=S_QUIT;
						
					
						}
					}else
					{
						buffer_in[recibidos]=0x00;														//__________________________Cada string debe acabar en 0x00 para indicar que acaba_________________
						printf(buffer_in);																//__________________________Sacamos por pantalla el buffer de entrada ____________________________
						if(estado!=S_DATA && strncmp(buffer_in,OK,2)==0)								//__________________________Aumentamos de estado en caso de no estar en estado S_DATA______________
							estado++;  
					}

				}while(estado!=S_QUIT);
				
	
		
			}
			else
			{
				printf("CLIENTE> ERROR AL CONECTAR CON %s:%d\r\n",ipdest,TCP_SERVICE_PORT);
			}		
			// fin de la conexion de transporte
			closesocket(sockfd);																		//------------------------------Primitiva CLOSESOCKET= cierra un socket existente -----------------
			
		}	
		printf("-----------------------\r\n\r\nCLIENTE> Volver a conectar (S/N)\r\n");
		option=_getche();																				//_____________________________Lee un caracter y lo muestra por pantalla______________

	}while(option!='n' && option!='N');

	
	
	return(0);

}
