#include <iostream>
#include <winsock2.h>
#include <stdio.h>

using namespace std;

class Client{
public:
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[4096];
    int op;
    int coneccion;
    Client(int puerto)
    {
        cout<<"Conectando al servidor..."<<endl<<endl;
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_addr.s_addr = inet_addr("192.168.0.79");
        addr.sin_family = AF_INET;
        addr.sin_port = htons(puerto);
        coneccion= connect(server, (SOCKADDR *)&addr, sizeof(addr));
        if (coneccion < 0) {
        cout<<"\n Error: No se pudo conectar al Servidor \n"<<endl;
        }else{
        cout << "Conectado al Servidor!" << endl;
        }
    }
    void Enviar()
    {
        this->op=-1;
        while(op<0 || op>3){
            cout<<"\n1. Realizar calculo"<<endl;
            cout<<"2. Ver registro de actividades"<<endl;
            cout<<"3. Cerrar sesion"<<endl;
            cin>>this->op;
            switch(op){
            case 1: cout<<"Ingrese una operacion: "<<endl;
                    fflush(stdin);
                    gets(buffer);
                    if(strcmp(buffer,"volver")==0){
                        op=-1;
                    }
                    break;
            case 2: strcpy(this->buffer, "registro");break;
            case 3: strcpy(this->buffer, "cerrar Socket");break;
            default: cout<<"Opcion invalida"<<endl;
            }
        }

        send(server, buffer, sizeof(buffer), 0);
        if(strcmp(this->buffer,"cerrar Socket")==0){
            CerrarSocket();
        }
        memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado!" << endl;

    }
    void Recibir()
    {
        recv(server, buffer, sizeof(buffer), 0);
        cout << buffer << endl;
        memset(buffer, 0, sizeof(buffer));
    }
    void CerrarSocket()
    {
       closesocket(server);
       WSACleanup();
       cout << "Socket cerrado." << endl << endl;
    }
};

int main()
{
    Client *Cliente = new Client(5000);
    while(true)
    {
        Cliente->Enviar();
        Cliente->Recibir();
    }
}
