/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbaghoug <nbaghoug@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 10:47:37 by nbaghoug          #+#    #+#             */
/*   Updated: 2024/05/15 16:57:52 by nbaghoug         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

int check_port(const std::string& port)
{
    int i = 0;
    int flag = 1;
    int port_value;
    while(port[i])
    {
        if(port[i] >= '0' && port[i] <= '9')
            i++;
        else
        {
            flag = -1;
            break;
        }
    }
    std::istringstream iss(port);
    iss >> port_value;
    if(flag == - 1 ||  (port_value  < 1024 || port_value > 65535))
        return 0;
    return 1;
}

int main(int argc ,char **argv)
{
    if(argc == 3)
    {

        signal(SIGPIPE, SIG_IGN);
        signal(SIGINT , server::sig_handler);
        signal(SIGQUIT , server::sig_handler);
        if(check_port(argv[1]) == 0)
        {
            std::cerr << "Invalid Port" << std::endl;  
            exit(1);
        }
        std::string first_arg(argv[1]);
        std::string second_arg(argv[2]);
        server ser(first_arg,second_arg);
        ser.create_server();
    }
    else
        std::cerr << "Error: usage --> ./ircserv <port> <password>" 
                    <<std::endl;
}