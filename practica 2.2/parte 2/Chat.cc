#include "Chat.h"

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
        
    char* it = _data;
    
    memcpy(it, &type, sizeof(uint8_t));
    it += sizeof(uint8_t);

    //nick ocupa 8
    char nick_c[8];
    strcpy(nick_c, nick.c_str());
    memcpy(it, nick_c, 8 * sizeof(char));
    it += 8 * sizeof(char);

    //message ocupa 80
    char message_c[80];
    strcpy(message_c, message.c_str());
    memcpy(it, message_c, 80 * sizeof(char));
    it += 80 * sizeof(char);

    //para testear los valores iniciales
    //std::cout << "to_bin(): " << type << " " << nick << " " << message << '\n';
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);


        char* it = _data;

        //el único caso de error de la función que se nos ocurre es que se
        //sobrepase el tamaño de data, de ahi que usemos un try/catch para lanzar
        //el return -1

        try
        {
            memcpy(&type, it, sizeof(uint8_t));
            it += sizeof(uint8_t);

            char nick_c[8];
            memcpy(nick_c, it, 8 * sizeof(char));
            nick = std::string(nick_c);
            it += 8 * sizeof(char);

            char message_c[80];
            memcpy(message_c, it, 80 * sizeof(char));
            message = std::string(message_c);
            it += 80 * sizeof(char);
        }
        catch(const std::exception& e)
        {
            std::cerr << "from_bin() fail: " << e.what() << '\n';
            return -1;
        }

        //para testear los valores obtenidos
        //std::cout << "from_bin(): " << type << " " << nick << " " << message << '\n';

        return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        std::cout << clients.size();

        ChatMessage m;
        Socket* sock;
        socket.recv(m, sock);

        switch(m.type)
        {
            case ChatMessage::LOGIN:
                clients.push_back(sock);
                std::cout << m.nick << " logged in from: " << *sock << ".\n";
                break;

            case ChatMessage::MESSAGE:        
                for(Socket* s : clients)
                {
                    if((*s) != (*sock))
                        socket.send(m, *s);
                }

                break;

            case ChatMessage::LOGOUT:
                for(auto it = clients.begin(); it != clients.end(); it++)
                {
                    if(sock == (*it))
                    {
                        clients.erase(it);
                std::cout << m.nick << " logged out from: " << *sock << ".\n";
                        break;
                    }
                }
                break;
            
            default:
                break;
        }

    }
}

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        std::string m;
        std::getline(std::cin, m);

        ChatMessage msg;
        msg.nick = nick;
        msg.type = ChatMessage::MESSAGE;
        msg.message = m;

        socket.send(msg, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        ChatMessage msg;
        
        socket.recv(msg);

        std::string m = msg.message;
        std::string n = msg.nick;
        std::cout << "(" << n << ") : " << m << '\n';
    }
}

