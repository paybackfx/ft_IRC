/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anas <anas@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 10:39:40 by nbaghoug          #+#    #+#             */
/*   Updated: 2024/06/18 01:07:14 by anas             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include <limits>


    int server::signal_has_recieve = false;
        server::server() 
        {
            this->port = 7777;
            clientManager = ClientManager::getInstance();
            this->password = "default_value";
            this->server_socket = -1;
        }
        
        server::server(const std::string& port,const std::string& password)
        {
            clientManager = ClientManager::getInstance();
            std::stringstream ss(port);
            ss >> this->port;
            this->password = password;
        }

        server::~server()
        {
            
        }

        server::server(server& server): port(server.port), password(server.password)
        {
            
        }

        void server::sig_handler(int signum)
        {
            signal_has_recieve = true;

            std::cout << "signal to CLOSE THE SERVER  has recieve " << signum << std::endl;
        }


        server& server::operator=(server& server)
        {
            if(this != &server)
            {
                this->port = server.port;
                this->password = server.password;
            }
            return *this;
        }

        void    server::create_socket()
        {
            this->server_socket = socket(AF_INET,SOCK_STREAM,0);
           int opt = 1;
            if (setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) 
            {
                std::cerr << "Failed to set SO_REUSEADDR\n";
                exit(1);

            }
    
            if(fcntl(this->server_socket , F_SETFL, O_NONBLOCK) == -1)
                throw "fcntl failed !";
         

        }


        void    server::bind_socket()
        {
            sockaddr_in serverAdd;
            serverAdd.sin_family = AF_INET;
            serverAdd.sin_port = htons(this->port);
            serverAdd.sin_addr.s_addr = INADDR_ANY;
            bind(this->server_socket,(struct sockaddr*)&serverAdd,sizeof(serverAdd));
        }
        
        void    server::create_server()
        {
            struct epoll_event ev;
            struct epoll_event evlist[100];
            create_socket();
            int nb;
            bind_socket();
            listen(this->server_socket, 100);
            
            ev.events =  EPOLLIN;
            ev.data.fd = this->server_socket;
            fd_epoll = epoll_create1(0);

            if(fd_epoll < 0)
            {
                std::cerr <<  "couldn't create an instance of epoll" << std::endl;
            }
            epoll_ctl(fd_epoll, EPOLL_CTL_ADD, this->server_socket, &ev);
           while (!signal_has_recieve) {
                    nb = epoll_wait(fd_epoll, evlist, 100, -1);
                    if (nb == -1) {
                        std::cerr << "SERVER WILL BE CLOSED SOON!" << std::endl;
                        break;
                    }

                    for (int i = 0; i < nb; ++i) {

                        if (this->server_socket == evlist[i].data.fd ) {
                            std::cout << "New connection detected on server socket." << std::endl;
                            add_connection();
                        } else if (evlist[i].events & EPOLLIN) {
                            handle_request(evlist[i].data.fd);
                        }
                    }
                }
            this->clientManager->free_all();
            close(this->server_socket);
            dprintf(1,"SOCKET CLOSED\n");
        }

        void server::add_connection()
        {
            struct sockaddr_in addr;
            struct epoll_event ev;
            socklen_t  addr_len = sizeof(addr);
            int client_socket;
            client_socket = accept(this->server_socket, (struct sockaddr *)&addr,
                                    &addr_len);
            if (client_socket == -1)
            {
                std::cout << "The server socket wasn't created successfully!" << std::endl;
                exit(1);
            }
            if(fcntl(client_socket , F_SETFL, O_NONBLOCK) == -1)
                throw "fcntl failed !";
            if(client_socket != -1)
            { 
                ev.events = EPOLLIN;
                ev.data.fd = client_socket;
                Client.push_back(client_socket);
                epoll_ctl(fd_epoll, EPOLL_CTL_ADD,client_socket, &ev);
                client* newClient = new client(client_socket , this->port, this->password);
                if(newClient)
                {
                    clientManager->addClient(client_socket, newClient);
                }
                else 
                    exit(1);
            }
        }

        void server::handle_request(int fd)
        {
            char buffer[1024];
            int recv_byte;
            memset(buffer,0,1024);
            recv_byte = recv(fd,buffer,sizeof(buffer) - 1, 0);
            client* currentClient = clientManager->getClient(fd);
            if(recv_byte > 0)
            {   
                std::string buff(buffer);
                this->mab[fd] +=  buff;
                if (this->mab[fd].find("\n") != std::string::npos)
                {
                    if(currentClient){
                        currentClient->check_cmd(fd, this->mab[fd]);
                    }
                    this->mab[fd] = "";
                }
                    
            }
            if (recv_byte <=  0)
            {   
                //std::cout << "Im here" <<std::endl;
                this->mab.erase(fd);
                if (epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd, NULL) == -1) 
                    std::cerr << "epoll_ctl: EPOLL_CTL_DEL" << std::endl;
                clientManager->removeClientFromChannels(fd);
                clientManager->removeClient(fd);
                close(fd);
            }
        }

