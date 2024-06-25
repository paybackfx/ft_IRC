/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anas <anas@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 12:37:11 by afennoun          #+#    #+#             */
/*   Updated: 2024/06/25 22:39:33 by anas             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"

client::client() : clientManager(ClientManager::getInstance()), fd(-1), connected(false) {

    command_map["connect"] = &client::connect;
    command_map["nick"] = &client::nick;
    command_map["user"] = &client::user;
    command_map["privmsg"] = &client::privmsg;
    command_map["notice"] = &client::privmsg;
    command_map["filemsg"] = &client::filemsg;
    command_map["quit"] = &client::quit;
    command_map["YES"] = &client::yes_no;
    command_map["NO"] = &client::yes_no;


    
}

client::client(int fd , unsigned int port, const std::string& password) : clientManager(ClientManager::getInstance()), fd(fd), port(port), password(password), connected(false) {

    command_map["connect"] = &client::connect;
    command_map["nick"] = &client::nick;
    command_map["user"] = &client::user;
    command_map["privmsg"] = &client::privmsg;
    command_map["filemsg"] = &client::filemsg;
    command_map["quit"] = &client::quit;
    command_map["YES"] = &client::yes_no;
    command_map["NO"] = &client::yes_no;
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

void client::set_save(const std::string save)
{
    this->save = save;
}

void client::set_confirme(int index ,bool confirme)
{
    this->confirme[index] = confirme;
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
bool client::get_confirme(int index)
{
    return (this->confirme[index]);
}
std::string client::get_save()
{
    return (this->save);
}
void client::clear_save()
{
    this->save.clear();
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
    if(get_connected())
    {
        dprintf(fd, "Error: you are already connected\n");
        return;
    }
    
    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token) {
        args.push_back(token);
    }

    if (args.size() != 4 && !(args[0] == "/connect" || args[0] == "connect")){
        dprintf(fd, "Error: invalid command or number of arguments\n");
        return;
    }

    if (get_connected()) {
        dprintf(fd, "Error: you are already connected\n");
        return;
    }

    if (get_password() != args[3] || get_port() != args[2]) {
        dprintf(fd, "Error: invalid password or port\n");
        return;
    }

    set_connected(true);
    dprintf(fd, "Connected\n");
}

void client::nick(int fd, const std::string& message) {
    if (!get_connected()) {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token) {
        args.push_back(token);
    }

    std::string currentNick = get_nickname();

    if (args.size() == 1 && (args[0] == "/nick" || args[0] == "nick")) {
        if (currentNick.empty()) {
            dprintf(fd, "ERR_NONICKNAMEGIVEN\n");
        } else {
            dprintf(fd, "Your nickname is: %s\n", currentNick.c_str());
        }
    } else if (args.size() == 2 && (args[0] == "/nick" || args[0] == "nick")) {
        std::string newNick = args[1];
        if (!isValideNickname(fd, newNick)) return;

        if (clientManager->nicknameUsed(newNick)) {
            dprintf(fd, "ERR_NICKNAMEINUSE\n");
        } else {
            set_nickname(newNick);
            dprintf(fd, "Your nickname has been changed to: %s\n", newNick.c_str());
        }
    } else if (args.size() == 3 && (args[1] == "/nick" || args[1] == "nick")) {
        std::string newNick = args[2];
        if (!isValideNickname(fd, newNick)) return;

        if (currentNick.empty()) {
            dprintf(fd, "ERR_NONICKNAMEGIVEN\n");
        } else if (args[0] != currentNick) {
            dprintf(fd, "ERR_NONMATCHNICKNAME\n");
        } else if (clientManager->nicknameUsed(newNick)) {
            dprintf(fd, "ERR_NICKNAMEINUSE\n");
        } else {
            set_nickname(newNick);
            dprintf(fd, "Your nickname has been changed to: %s\n", newNick.c_str());
        }
    } else if (args.size() == 2 && args[1] == "/nick") {
        dprintf(fd, "ERR_NONICKNAMEGIVEN\n");
    } else {
        dprintf(fd, "Error: invalid command\n");
    }
}

void client::user(int fd, const std::string& message) {
    if (!get_connected()) {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token) {
        args.push_back(token);
    }

    if (args.size() != 5 && !(args[0] == "/user" || args[0] == "user")) {
        dprintf(fd, "ERR_NEEDMOREPARAMS\n");
        return;
    }

    if (get_registered()) {
        dprintf(fd, "ERR_ALREADYREGISTRED\n");
        return;
    }

    std::string username = args[1];
    std::string hostname = args[2];
    std::string servername = args[3];
    std::string realname = args[4];

    if (username.empty() || hostname.empty() || servername.empty() || realname.empty()) {
        dprintf(fd, "ERR_NEEDMOREPARAMS\n");
        return;
    }

    set_username(username);
    set_hostname(hostname);
    set_servername(servername);
    set_realname(realname);
    set_registered(true);

    dprintf(fd, "User registration completed: %s\n", username.c_str());
}


void client::quit(int fd, const std::string& message) {
    if (!get_connected()) {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token) {
        args.push_back(token);
    }

    if (args.empty() && !( args[0] == "/quit" || args[0] == "quit")){
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (args.size() == 1) {
        set_connected(false);
        dprintf(fd, "Disconnected from %s:%s\n", get_adress_ip().c_str(), get_port().c_str());
        return;
    }

    if (args.size() > 1) {
        size_t pos = message.find(":");
        if (pos != std::string::npos) {
            std::string reason = message.substr(pos + 1);
            clientManager->broadcastToAll(get_nickname() + " has quit (" + reason + ")", fd);
            set_connected(false);
            return;
        }
    }

    dprintf(fd, "Error: invalid command\n");
}


void client::privmsg(int fd, const std::string& message) {
    if (!get_connected()) {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token) {
        args.push_back(token);
    }

    if (args.empty() && !( args[0] == "/privmsg" || args[0] == "privmsg")){
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (args.size() < 3) {
        dprintf(fd, "Error: invalid number of arguments\n");
        return;
    }

    std::string recipient = args[1];
    size_t pos = message.find(":");
    if (pos == std::string::npos) {
        dprintf(fd, "Error: invalid message\n");
        return;
    }

    std::string msg = message.substr(pos + 1);

    if (recipient[0] == '#') {
        // Code to handle messages to channels, e.g.:
        // clientManager->sendMessageToChannel(recipient, msg, fd);
    } else {
        client* recipientClient = clientManager->getClient(recipient);
        if (recipientClient && recipientClient->get_fd() != fd) {
            dprintf(recipientClient->get_fd(), ":%s PRIVMSG %s :%s\n", get_nickname().c_str(), recipient.c_str(), msg.c_str());
        } else {
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

    if (!(command == "/filemsg" || command == "filemsg")) {
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

    client* recipientClient = clientManager->getClient(recipient);
    recipientClient->set_confirme(0, false);
    recipientClient->set_confirme(1, false);
    if (!recipientClient || recipientClient->get_fd() == fd) {
        dprintf(fd, "Error: user not found\n");
        return;
    }

    
    char buffer[4096];
    std::string file_contents;
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        file_contents.append(buffer, file.gcount());
    }

    recipientClient->set_save(file_contents);
    recipientClient->set_confirme(0, true);
    dprintf(recipientClient->get_fd(),"%s user has sent you a file named %s, do you want to accept it? (YES/NO)\n", get_nickname().c_str(), filename.c_str());

    file.close();
}


void client::yes_no(int fd, const std::string& message) {
    if (get_connected() == false) {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }
    client* recipientClient = clientManager->getClient(fd);
    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";

    if (command != "YES" && command != "NO" ){
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (count != 1) {
        dprintf(fd, "Error: invalid number of arguments\n");
        return;
    }

    if (command == "YES" && recipientClient->get_confirme(0)) {
        recipientClient->set_confirme(1, true); 
        dprintf(fd, "File transfer accepted\n");
    } else if (command == "NO" && recipientClient->get_confirme(0)){
        dprintf(fd, "File transfer refused\n");
        recipientClient->set_confirme(0, false);
        recipientClient->set_confirme(1, false);
    } else {
        dprintf(fd, "Error: invalid response\n");
    }
    if (recipientClient->get_confirme(0) && recipientClient->get_confirme(1)) {
        dprintf(fd, "%s", recipientClient->get_save().c_str());
        dprintf(fd, "\nfile transfer completed\n");
    }
    recipientClient->clear_save();
}
client::~client()
{
    set_connected(false);
}
