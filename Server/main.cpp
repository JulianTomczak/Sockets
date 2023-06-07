#include <iostream>
#include <winsock2.h>
#include <math.h>
#include <ctime>
#include <fstream>
#include <string>

using namespace std;

class Server{
public:
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[4096];
    bool close;
    double t1;
    double t2;
    fstream archivo;

    //Fecha y hora actual
    string tiempo(){
        time_t t = time(nullptr);
        tm* now = localtime(&t);

        char buffer[128];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d_%X", now);
        return buffer;
    }
    Server()
    {
        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(5000);

        archivo.open("server.log",ios::app);
        archivo<<tiempo()<<": ============================="<<endl;
        archivo<<tiempo()<<": =======Inicia Servidor======="<<endl;
        archivo<<tiempo()<<": ============================="<<endl;
        archivo<<tiempo()<<": Socket creado. Puerto de escucha:5000."<<endl;
        archivo.close();

    }
    void Espera(){
        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));
        listen(server, 0);

        cout << "Escuchando para conexiones entrantes." << endl;
        int clientAddrSize = sizeof(clientAddr);
        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {
            cout << "Cliente conectado!" << endl;
            archivo.open("server.log",ios::app);
            archivo<<tiempo()<<": Conexion Aceptada."<<endl;
            archivo.close();
            close=false;
            t1=clock(); //empieza a correr el tiempo de conexion
        }
    }

    string Recibir()
    {
      recv(client, buffer, sizeof(buffer), 0);
      cout << "El cliente dice: " << buffer << endl;

      if(strcmp(buffer,"registro")!=0){ // con el comando registro pasa a enviar
      t2=clock(); //tiempo actual
      //Verifico si estubo 2 min sin actividad
      if(((t2-t1)/CLOCKS_PER_SEC)>120){
         close=true;
         strcpy(buffer,"Cliente desconectado por inactividad");
         archivo.open("server.log",ios::app);
         archivo<<tiempo()<<": Conexion Cerrada por Inactividad."<<endl;
         archivo.close();
         Enviar();
         CerrarSocket();
      }else{ //si no estubo 2 min inactivo el tiempo de conexion t1 pasa a ser el actual
          t1=clock();
          t2=clock();
      }
      //si llega el comando cerrar Socket se cierra
      if(strcmp(buffer,"cerrar Socket")==0){
            close=true;
            archivo.open("server.log",ios::app);
            archivo<<tiempo()<<": Cliente desconectado."<<endl;
            archivo.close();
            CerrarSocket();
      }
      char numero1[20]=" ";
      char numero2[20]=" ";
      char operador;
      int cambio=0;
      int cont=0;
      int operacion=0;
      bool factorial=false;
      int i=0;
      bool error=false;
      bool error1=false;
      string msj(buffer);
      //verifico si tiene entre 1 y 20 caracteres
      if(msj.length()>20 | msj.length()<1){
        string mensaje="La operacion debe tener entre 1 y 20 caracteres";
        strcpy(buffer, mensaje.c_str());
        error=true;
        error1=true;
      }
      //verifico si hay caracteres no contemplados
      while(i<(msj.length()) && error==false){
        if(!(buffer[i]=='0' | buffer[i]=='1' | buffer[i]=='2' | buffer[i]=='3' | buffer[i]=='4' | buffer[i]=='5'
             | buffer[i]=='6' | buffer[i]=='7' | buffer[i]=='8' | buffer[i]=='9'
             | buffer[i]=='+'| buffer[i]=='-'| buffer[i]=='*'| buffer[i]=='/'|
               buffer[i]=='^'| buffer[i]=='!')){
               string texto="No se pudo realizar la operacion, se encontro un caracter no contemplado: ";
               char caracter=buffer[i];
               string aux(1,caracter);
               string mensaje;
               mensaje.append(texto).append(aux);
               strcpy(buffer, mensaje.c_str());
               error=true;
               error1=true;
           }
           i++;
      }
      i=0;
      //guardo los numeros y el operador si esta mal formada error pasa a true
      while(i<(msj.length()) && error==false){

          if(buffer[i]=='0' | buffer[i]=='1' | buffer[i]=='2' | buffer[i]=='3' | buffer[i]=='4' | buffer[i]=='5'
             | buffer[i]=='6' | buffer[i]=='7' | buffer[i]=='8' | buffer[i]=='9'){
                if(cambio==0){
                   numero1[i]=buffer[i];
                   cont++;
                }
                if(cambio==1){
                   if(factorial==true | cont==1){
                    error=true;
                   }
                   numero2[i-cont]=buffer[i];
                }
          }

         if(buffer[i]=='+'| buffer[i]=='-'| buffer[i]=='*'| buffer[i]=='/'|
               buffer[i]=='^'| buffer[i]=='!'){
            operador=buffer[i];
            cambio=1;
            if(buffer[i]=='!'){
                factorial=true;
            }
            cont++;
            operacion++;
            if(operacion>1){
                error=true;
            }
            }

          i++;
      }
      //error=true -> mal formada, error1=true -> caracteres no contemplados
      if(error==true && error1==false){
        string text="No se pudo realizar la operacion, la operacion esta mal formada: ";
        string caracter(1,buffer[i-1]);
        string caracterAnt;
        if(i>0){
            caracterAnt= buffer[i-2];
            }
        string caracterPost(1,buffer[i]);
        string mensaje;
        mensaje.append(text).append(caracterAnt).append(caracter).append(caracterPost);
        strcpy(buffer, mensaje.c_str());
      }
      //Si no hay errores comienzo la calculadora
      if(error==false){
      int n1 = atoi(numero1);
      int n2= atoi(numero2);
      switch(operador){
          case '+':{
              int res=n1+n2;
              string tmp = to_string(res);
              char const *num_char = tmp.c_str();
              strcpy(buffer,"Resultado: ");
              strcat(buffer,num_char);
              break;
              }
          case '-':{
              int res=n1-n2;
              string tmp = to_string(res);
              char const *num_char = tmp.c_str();
              strcpy(buffer,"Resultado: ");
              strcat(buffer,num_char);
              break;
              }
          case '*':{
              int res=n1*n2;
              string tmp = to_string(res);
              char const *num_char = tmp.c_str();
              strcpy(buffer,"Resultado: ");
              strcat(buffer,num_char);
              break;
              }
          case '/':{
              int res=n1/n2;
              string tmp = to_string(res);
              char const *num_char = tmp.c_str();
              strcpy(buffer,"Resultado: ");
              strcat(buffer,num_char);
              break;
              }
          case '^':{
              int res= pow(n1, n2);
              string tmp = to_string(res);
              char const *num_char = tmp.c_str();
              strcpy(buffer,"Resultado: ");
              strcat(buffer,num_char);
              break;
              }
          case '!':{
              int factorial=1;
              for(i=1;i<=n1;i++)
              factorial = factorial * i;
              string tmp = to_string(factorial);
              char const *num_char = tmp.c_str();
              strcpy(buffer,"Resultado: ");
              strcat(buffer,num_char);
              break;
              }
          default: cout<<" "<<endl;
      }
      }
      }
    }

    void Enviar()
    {
        // con el comando registro copio el archivo al buffer
        if(strcmp(buffer,"registro")==0){
           string t;
           string s;
           archivo.open("server.log",ios::in);
           while(!archivo.eof())
           {
            getline(archivo,s);
            t.append(s + '\n');
           }
           archivo.close();
           strcpy(buffer, t.c_str());
        }
        //envio el buffer
        send(client, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado!" << endl;

    }
    void CerrarSocket()
    {
        closesocket(client);
        cout << "Socket cerrado, cliente desconectado." << endl;
    }
};


int main()
{

  Server *Servidor = new Server();
  while(true){
    Servidor->Espera();
    Servidor->close=false;
  while(Servidor->close==false)
  {
     Servidor->Recibir();
     Servidor->Enviar();
  }
  }

}
