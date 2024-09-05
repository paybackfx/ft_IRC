/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbaghoug <nbaghoug@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 10:39:37 by nbaghoug          #+#    #+#             */
/*   Updated: 2024/05/19 10:15:31 by nbaghoug         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <vector>
#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>

class server {

    public:
            std::vector<int> client;
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
};