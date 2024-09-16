/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbaghoug <nbaghoug@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 10:39:40 by nbaghoug          #+#    #+#             */
/*   Updated: 2024/05/19 10:39:59 by nbaghoug         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

        server::server()
        {
            this->port = 7777;
            this->password = "default_value";
            this->server_socket = -1;
        }
        
        server::server(const std::string& port,const std::string& password)
        {
            std::stringstream ss(port);
            ss >> this->port;
            std::cout << this->port << std::endl;
            this->password = password;
        }

        server::~server()
        {
            
        }

        server::server(server& server): port(server.port), password(server.password)
        {

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
            
            ev.events = EPOLLIN;
            ev.data.fd = this->server_socket;
            int fd_epoll = epoll_create1(EPOLL_CLOEXEC);

            if(fd_epoll < 0)
            {
                std::cerr <<  "couldn't create an instance of epoll" << std::endl;
            }
            epoll_ctl(fd_epoll, EPOLL_CTL_ADD, this->server_socket, &ev);
            while(true)
            {
                nb =  epoll_wait(fd_epoll, evlist, 100, -1);
                std::cout << nb << std::endl;
                if(nb == -1)
                    std::cerr <<  "epoll wait error" << std::endl;
                for(int i = 0;i < nb;i++)
                {       
                    if(this->server_socket == evlist[i].data.fd)
                    {
                        add_connection();
                    }
                    else
                    {
                      handle_request(evlist[i].data.fd);
                    }
                }
            }
        }

        void server::add_connection()
        {
            struct sockaddr_in addr;
            struct epoll_event ev;
            socklen_t  addr_len = sizeof(addr);
            int client_socket;
            client_socket = accept(this->server_socket, (struct sockaddr *)&addr,
                                    &addr_len);
            if(fcntl(this->server_socket , F_SETFL, O_NONBLOCK) == -1)
                throw "fcntl failed !";
            if(client_socket != -1)
            {
                ev.events = EPOLLIN;
                ev.data.fd = client_socket;
                client.push_back(client_socket);
                epoll_ctl(this->fd_epoll, EPOLL_CTL_ADD,client_socket, &ev);
            }
        }

        void server::handle_request(int fd)
        {
            char buffer[1024];
            int recv_byte;
            recv_byte = recv(fd,buffer,sizeof(buffer),MSG_DONTWAIT);
            if(recv_byte > 0)
            {
                std::string buff(buffer);
                this->mab[fd] +=  buff;
                if (this->mab[fd].find("\n") != std::string::npos)
                {
                    send(fd,this->mab[fd].c_str(),this->mab[fd].size(),MSG_DONTWAIT);      
                    this->mab[fd] = "";
                }
                    
            }
            else
            {
                if (epoll_ctl(fd_epoll, EPOLL_CTL_DEL, fd, NULL) == -1) 
                    std::cerr << "epoll_ctl: EPOLL_CTL_DEL" << std::endl;
            }
        }