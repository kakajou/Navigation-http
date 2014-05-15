    #define LINUX_OS
    // #define WIN_OS
    #define _DEBUG_PRINT(X)   /* X */
    //For commn
    #include <iostream>
    #include <fstream>
    #include <string>
    #include <stdlib.h>
    #include <assert.h>
    #ifdef LINUX_OS
    #include <netdb.h>
    #endif
    #ifdef WIN_OS
    #include <Winsock2.h>
    #endif
    #define SEND_RQ(MSG) \
                    /*cout<<send_str;*/ \
      send(sock,MSG,strlen(MSG),0);
    using namespace std;
    //<exe> hostname api parameters
    int request (char* hostname, char* api, char* parameters, string& message)
    {
    #ifdef WIN_OS
    {
     WSADATA WsaData;
     WSAStartup (0x0101, &WsaData);
    }
    #endif
        sockaddr_in       sin;
        int sock = socket (AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
     return -100;
    }
        sin.sin_family = AF_INET;
        sin.sin_port = htons( (unsigned short)80);
        struct hostent * host_addr = gethostbyname(hostname);
        if(host_addr==NULL) {
          _DEBUG_PRINT( cout<<"Unable to locate host"<<endl );
          return -103;
        }
        sin.sin_addr.s_addr = *((int*)*host_addr->h_addr_list) ;
        _DEBUG_PRINT( cout<<"Port :"<<sin.sin_port<<", Address : "<< sin.sin_addr.s_addr<<endl);
        if( connect (sock,(const struct sockaddr *)&sin, sizeof(sockaddr_in) ) == -1 ) {
         _DEBUG_PRINT( cout<<"connect failed"<<endl ) ;
         return -101;
        }
        cout << "\nadr: " << sin.sin_addr.s_addr << "\n" << endl;
    string send_str;
    SEND_RQ("POST " );
    SEND_RQ(api);
    SEND_RQ(" HTTP/1.0\r\n" );
    SEND_RQ("Accept: */*\r\n" );
    SEND_RQ("User-Agent: Mozilla/4.0\r\n" );
    char content_header[100];
    sprintf(content_header,"Content-Length: %d\r\n",strlen(parameters));
    SEND_RQ(content_header);
    SEND_RQ("Accept-Language: en-us\r\n" );
    SEND_RQ("Accept-Encoding: gzip, deflate\r\n" );
    SEND_RQ("Host: " );
    SEND_RQ("hostname" );
    SEND_RQ("\r\n" );
    SEND_RQ("Content-Type: application/x-www-form-urlencoded\r\n" );
    //If you need to send a basic authorization
    //string Auth        = "username:password";
    //Figureout a way to encode test into base64 !
    //string AuthInfo    = base64_encode(reinterpret_cast<const unsigned char*>(Auth.c_str()),Auth.length());
    //string sPassReq    = "Authorization: Basic " + AuthInfo;
    //SEND_RQ(sPassReq.c_str());
    SEND_RQ("\r\n" );
    SEND_RQ("\r\n" );
    SEND_RQ(parameters);
    SEND_RQ("\r\n" );
    _DEBUG_PRINT(cout<<"####HEADER####"<<endl);
    char c1[1];
    int l,line_length;
    bool loop = true;
    bool bHeader = false;
    while(loop) {
       l = recv(sock, c1, 1, 0);
       if(l<0) loop = false;
       if(c1[0]=='\n') {
           if(line_length == 0) loop = false;
           line_length = 0;
           if(message.find("200" ) != string::npos)
           bHeader = true;
       }
       else if(c1[0]!='\r') line_length++;
       _DEBUG_PRINT( cout<<c1[0]);
       message += c1[0];
    }
    message="";
    if(bHeader) {
         _DEBUG_PRINT( cout<<"####BODY####"<<endl) ;
         char p[1024];
         while((l = recv(sock,p,1023,0)) > 0)  {
             _DEBUG_PRINT( cout.write(p,l)) ;
         p[l] = '\0';
         message += p;
         }
         _DEBUG_PRINT( cout << message.c_str());
    } else {
     return -102;
    }
    #ifdef WIN_OS
       WSACleanup( );
    #endif
    return 0;
    }
    int main(int argc, char *argv[])
    {
    // 2 parametres doivent etre passés au prog:
    // - l'url de retour sur le site
    // - la variable data contenant le resultat de RESPONSE
    cout << "argc: " << argc << endl;
    char* url;
    char* param;
    string message;
    int codeRetour;
    if ( argc > 1 )
    {
     for (int i=0; i<= argc; i++)
     {
      //cout<<"arg "<< i <<"= "<< argv[i] <<endl;
      switch(i)
      {
       case 0 :cout << "- nom / chemin exec:" << argv[i] << endl; break;
       case 1 :cout << "- param_n 1: url = " << argv[i] << endl;
        url= argv[i];
        break;
       case 2 :cout << "- param_n 2: data= " << argv[i] << endl;
        param= argv[i];
        break;
       default : cout << "param_supplementaire: " << endl;
      }
     }
     codeRetour= request (url, "/post_url.pl", param, message);
     // message contains response!
     /* Ouverture du fichier  en mode ecriture */
     ofstream fichier("res.html" );
     if (!fichier) {
     cerr << "Impossible d'ouvrir le fichier!" << endl;
     return (-1);
     }
     /* Désormais on peut écrire dans le fichier comme avec cout */
     fichier << message << endl;
     /* Fermeture du fichier */
     fichier.close();;
     cout << "resultat:\ncode retour: " << codeRetour << "\nmessage:"<< message << endl;
    }
    else
    {
     cout << "pas de parametres. " << endl;
     return -1;
     // erreur: pas de parametres passé au programme
     // on ne peut rediriger vers le site
    }
     
    }