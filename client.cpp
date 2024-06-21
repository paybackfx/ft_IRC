/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anas <anas@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 12:37:11 by afennoun          #+#    #+#             */
/*   Updated: 2024/06/21 04:22:02 by anas             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

client::client() : clientManager(ClientManager::getInstance()), fd(-1), connected(false) {

    command_map["/connect"] = &client::connect;
    command_map["/nick"] = &client::nick;
    command_map["/user"] = &client::user;
    command_map["/privmsg"] = &client::privmsg;
    command_map["/notice"] = &client::privmsg;
    command_map["/filemsg"] = &client::filemsg;
    commmand_map["/filestart"] = &client::handle_file_start;
    // command_map["/filedata"] = &client::handle_file_data;
    // command_map["/fileend"] = &client::handle_file_end;
    command_map["/quit"] = &client::quit;
}

client::client(int fd , unsigned int port, const std::string& password) : clientManager(ClientManager::getInstance()), fd(fd), port(port), password(password), connected(false) {

    command_map["/connect"] = &client::connect;
    command_map["/nick"] = &client::nick;
    command_map["/user"] = &client::user;
    command_map["/privmsg"] = &client::privmsg;
    command_map["/filemsg"] = &client::filemsg;
    commmand_map["/filestart"] = &client::handle_file_start;
    // command_map["/filedata"] = &client::handle_file_data;
    // command_map["/fileend"] = &client::handle_file_end;
    command_map["/quit"] = &client::quit;
}
client::client(const client &src)
{
    this->connected = src.connected;
    this->fd = src.fd;
    this->adress_ip = src.adress_ip;
    this->port = src.port;
    this->nickname = src.nickname;
    this->username = src.username;
}

client &client::operator=(const client &src)
{
    this->connected = src.connected;
    this->fd = src.fd;
    this->adress_ip = src.adress_ip;
    this->port = src.port;
    this->nickname = src.nickname;
    this->username = src.username;
    return (*this);
}

//-----------------SETTERS-----------------//

void client::set_connected(bool connected)
{
    this->connected = connected;
}

void client::set_registered(bool is_registered)
{
    this->is_registered = is_registered;
}
void client::set_operator(bool _operator)
{
    this->_operator = _operator;
}

void client::set_fd(int fd)
{
    this->fd = fd;
}

void client::set_adress_ip(const std::string& adress_ip)
{
    this->adress_ip = adress_ip;
}

void client::set_port(unsigned int port)
{
    this->port = port;
}

void client::set_nickname(const std::string nickname)
{
    this->nickname = nickname;
}

void client::set_username(const std::string username)
{
    this->username = username;
}

void client::set_realname(const std::string realname)
{
    this->realname = realname;
}

void client::set_hostname(const std::string hostname)
{
    this->hostname = hostname;
}
void client::set_servername(const std::string servername)
{
    this->servername = servername;
}
void client::set_password(std::string password)
{
    this->password = password;
}
void client::set_chanels(std::string chanel)
{
    this->chanels.insert(chanel);
}

void client::set_modes(std::string mode)
{
    this->modes.insert(mode);
}

void client::del_chanels(std::string chanel)
{
    this->chanels.erase(chanel);
}

void client::del_modes(std::string mode)
{
    this->modes.erase(mode);
}

//-----------------GETTERS-----------------//
std::string client::get_adress_ip()
{
    return (this->adress_ip);
}

std::string client::get_port()
{
    char buffer[6]; 
    sprintf(buffer, "%u", this->port); 
    return std::string(buffer);
}

std::string client::get_password()
{
    return (this->password);
}
std::string client::get_nickname()
{
    return (this->nickname);
}

std::string client::get_username()
{
    return (this->username);
}

bool client::get_connected()
{
    return (this->connected);
}

bool client::get_registered()
{
    return (this->is_registered);
}
bool client::is_operator()
{
    return (this->_operator);
}

int client::get_fd()
{
    return (this->fd);
}

//-----------------commands-----------------//

void client::check_cmd(int fd, const std::string& input) {
    std::map<std::string, CommandHandler>::iterator it;
    for (it = command_map.begin(); it != command_map.end(); ++it) {
        if (input.find(it->first) != std::string::npos) {
            CommandHandler handler = it->second;
            (this->*handler)(fd, input);
            return;
        }
    }
    std::cerr << "Unknown command received from client " << fd << ": " << input << std::endl;
    dprintf(fd, "Error: unknown command\n");
}

void client::connect(int fd, const std::string& message) {
    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string firstArg = count > 1 ? get_word(message, 1) : "";
    std::string secondArg = count > 2 ? get_word(message, 2) : "";
    std::string thirdArg = count > 3 ? get_word(message, 3) : "";

    
    if (command != "/connect") {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (count != 4) {
        dprintf(fd, "Error: invalid number of arguments\n");
        return;
    }

    if (get_connected()) {
        dprintf(fd, "Error: you are already connected\n");
        return;
    }
    
    if(get_password() != thirdArg || get_port() != secondArg)
    {
        dprintf(fd, "Error: invalid password or port\n");
        return;
    }
 
    set_connected(true);
    dprintf(fd, "Connected\n");
}


void client::nick(int fd, const std::string& message) {
    
    if(get_connected() == false)
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }
    int count = count_words(message);
    std::string firstWord = count > 0 ? get_word(message, 0) : "";
    std::string secondWord = count > 1 ? get_word(message, 1) : "";
    std::string thirdWord = count > 2 ? get_word(message, 2) : "";
    std::string currentNick = get_nickname();

    if (count == 1 && firstWord == "/nick") {
        if (currentNick.empty()) {
            dprintf(fd, "ERR_NONICKNAMEGIVEN\n");
        } else {
            dprintf(fd, "Your nickname is: %s\n", currentNick.c_str());
        }
    }
    else if (count == 2 && firstWord == "/nick") {
        std::string newNick = secondWord;
        if (!isValideNickname(fd, newNick)) return;

        if (clientManager->nicknameUsed(newNick)) {
            dprintf(fd, "ERR_NICKNAMEINUSE\n");
        } else {
            set_nickname(newNick);
            dprintf(fd, "Your nickname has been changed to: %s\n", newNick.c_str());
        }
    }
    else if (count == 3 && secondWord == "/nick") {
        std::string newNick = thirdWord;
        if (!isValideNickname(fd, newNick)) return;

        if (currentNick.empty()) {
            dprintf(fd, "ERR_NONICKNAMEGIVEN\n");
        } else if (firstWord != currentNick) {
            dprintf(fd, "ERR_NONMATCHNICKNAME\n");
        } else if (clientManager->nicknameUsed(newNick)) {
            dprintf(fd, "ERR_NICKNAMEINUSE\n");
        } else {
            set_nickname(newNick);
            dprintf(fd, "Your nickname has been changed to: %s\n", newNick.c_str());
        }
    }
    else if (count == 2 && secondWord == "/nick") {
        dprintf(fd, "ERR_NONICKNAMEGIVEN\n");
    }
    else {
        dprintf(fd, "Error: invalid command\n");
    }
}

void client::user(int fd, const std::string& message) {
    
    if(get_connected() == false)
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }
    
    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string username = count > 1 ? get_word(message, 1) : "";
    std::string hostname = count > 2 ? get_word(message, 2) : "";
    std::string servername = count > 3 ? get_word(message, 3) : "";
    std::string realname = count > 4 ? get_word(message, 4) : "";

    if (command != "/user") {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (count != 5) {
        dprintf(fd, "ERR_NEEDMOREPARAMS\n");
        return;
    }

    if (username.empty() || hostname.empty() || servername.empty() || realname.empty()) {
        dprintf(fd, "ERR_NEEDMOREPARAMS\n");
        return;
    }

    if (get_registered()) {
        dprintf(fd, "ERR_ALREADYREGISTRED\n");
        return;
    }

    set_username(username);
    set_hostname(hostname);
    set_servername(servername);
    set_realname(realname);

    set_registered(true);

    dprintf(fd, "User registration completed: %s\n", username.c_str());
}

void client::quit(int fd , const std::string& message)
{
    if(get_connected() == false)
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }
    int count = count_words(message);
    if (count == 1 && get_word(message, 0) == "/quit")
    {
        set_connected(false);
        dprintf(fd, "Disconnected from %s:%s\n", get_adress_ip().c_str(), get_port().c_str());
        return;
    }
    else if (count > 1 && get_word(message, 0) == "/quit") 
    {
        size_t pos = message.find(":");
        std::string reason = message.substr(pos + 1);
        if(pos != std::string::npos)
        {
            clientManager->broadcastToAll(get_nickname() + " has quit (" + reason + ")", fd);
            set_connected(false);  
        }
    }
    else
    {
        dprintf(fd, "Error: invalid command\n");
        return;
    }
}

void client::privmsg(int fd, const std::string& message)
{
    if(get_connected() == false )
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }
    int count = count_words(message);
    std :: string command = count > 0 ? get_word(message, 0) : "";
    std:: string recipient = count > 1 ? get_word(message, 1) : "";
    std:: string msg;
    
    if (command != "/privmsg")
    {
        dprintf(fd, "Error: invalid command\n");
        return;
    }
    if (count < 3)
    {
        dprintf(fd, "Error: invalid number of arguments\n");
        return;
    }
    size_t pos = message.find(":"); 
    if (pos != std::string::npos)
    {
        msg = message.substr(pos + 1);
    }
    else
    {
        dprintf(fd, "Error: invalid message\n");
        return;
    }

    if(recipient[0] == '#')
    {//waiting chanels ;
    }
    else
    {
        client* recipientClient = clientManager->getClient(recipient);
        if(recipientClient && recipientClient->get_fd() != fd){
            dprintf(recipientClient->get_fd(), ":%s PRIVMSG %s :%s\n", get_nickname().c_str(), recipient.c_str(), msg.c_str());
        }
        else
        {
            dprintf(fd, "Error: user not found\n");
        }
    }
}

void client::filemsg(int fd, const std::string& message) {
    if (get_connected() == false) {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }
    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string recipient = count > 1 ? get_word(message, 1) : "";
    std::string filename = count > 2 ? get_word(message, 2) : "";

    if (command != "/filemsg") {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (count != 3) {
        dprintf(fd, "Error: invalid number of arguments\n");
        return;
    }

    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        dprintf(fd, "Error: file not found\n");
        return;
    }

    file.seekg(0, std::ios::end);
    size_t filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    client* recipientClient = clientManager->getClient(recipient);
    if (!recipientClient || recipientClient->get_fd() == fd){
        dprintf(fd, "Error: user not found\n");
        return;
    }

    dprintf(recipientClient->get_fd(), ":%s /filestart %s %s %zu\n", get_nickname().c_str(), recipient.c_str(), filename.c_str(), filesize);

    bool confirmation_received = true; // a refaire

    if (confirmation_received) {
        char buffer[4096];
        while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
            std::string data(buffer, file.gcount());
            dprintf(recipientClient->get_fd(), ":%s filedata %s %s :%s\n", get_nickname().c_str(), recipient.c_str(), filename.c_str(), data.c_str());
        }

        dprintf(recipientClient->get_fd(), ":%s fileend %s %s\n", get_nickname().c_str(), recipient.c_str(), filename.c_str());
    }

    file.close();
}


client::~client()
{
    set_connected(false);
}
