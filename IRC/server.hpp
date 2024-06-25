/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anas <anas@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 10:39:37 by nbaghoug          #+#    #+#             */
/*   Updated: 2024/06/18 00:38:17 by anas             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>

#include "../clientM.hpp"
#include "../client.hpp"
class client;
class server {

    public:
            ClientManager* clientManager;
            std::vector<int> Client;
            std::map<int,std::string> mab;
            unsigned int port;
            std::string password;
            int server_socket;
            int fd_epoll;
    public:
            server();
            server(const std::string& port,const std::string& password);
            ~server();
            server(server& server);
            server& operator=(server& server);
            void    create_socket();
            void    bind_socket();
            void    create_server();
            void    add_connection();
            void    handle_request(int fd);

            //-----------------GETTERS-----------------//       
        
};