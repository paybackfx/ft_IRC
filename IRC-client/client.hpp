/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anas <anas@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 12:36:16 by afennoun          #+#    #+#             */
/*   Updated: 2024/06/19 02:29:51 by anas             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <set>
#include <istream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <iterator>
#include <algorithm>
#include <functional>
#include "clientM.hpp"

class server;
class ClientManager;
class client
{
    private:
        
        typedef void (client::*CommandHandler)(int, const std::string&);
        std::map<std::string, CommandHandler> command_map;
        ClientManager* clientManager;
        int fd;
        bool connected;
        bool is_registered;
        bool _operator;
        std::string adress_ip;
        unsigned int port;
        std::string nickname; // (9) caractères max
        std::string username; // (9) caractères max
        std::string realname;
        std::string hostname;
        std::string servername;
        std::string password;
        std::set<std::string> chanels;
        std::set<std::string> modes; // +i invite only  +o opertator  +t change topic +l limit users 
    
    public:
        client();
        client(int fd , unsigned int port, const std::string& get_password);
        client(const client &src);
        client &operator=(const client &src);
        ~client();
        
        //-----------------SETTERS-----------------//
        void set_connected(bool connected);
        void set_registered(bool is_registered);
        void set_fd(int fd);
        void set_adress_ip(const std::string& adress_ip);
        void set_port(unsigned int port);
        void set_nickname(const std::string nickname);
        void set_username(const std::string username);
        void set_hostname(const std::string hostname);
        void set_servername(const std::string servername);
        void set_realname(const std::string realname);
        void set_password(const std::string password);
        void set_operator(bool _operator);
        void set_chanels(std::string chanel);
        void set_modes(std::string mode);
        void del_chanels(std::string chanel);
        void del_modes(std::string mode);
        bool is_operator();

        //-----------------GETTERS-----------------//

        int get_fd();
        bool get_connected();
        bool get_registered();
        std::string get_adress_ip();
        std::string get_port();
        std::string get_nickname();
        std::string get_username();
        std::string get_password();
    

        //-----------------commands-----------------//
        void check_cmd(int fd, const std::string& input);
        void connect(int fd , const std::string& message);
        void nick(int fd , const std::string& message);
        void user(int fd , const std::string& message);
        void quit(int fd , const std::string& message);
        void privmsg(int fd , const std::string& message);
        
};

//-----------------utils-----------------//
int count_words(const std::string& str);
std::string get_word(const std::string& str, int index);
bool isValideNickname(int fd, std::string nickname);


#endif