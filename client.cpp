/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afennoun <afennoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/04 12:37:11 by afennoun          #+#    #+#             */
/*   Updated: 2024/09/05 16:18:22 by afennoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "clientM.hpp"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include "IRC/channel.hpp"

// ANSI color codes
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

// std::string server_hostname = "localhost";

client::client() : clientManager(ClientManager::getInstance()), fd(-1), connected(false)
{

    confirme[0] = false;
    confirme[1] = false;
    command_map["CONNECT"] = &client::connect;
    command_map["NICK"] = &client::nick;
    command_map["USER"] = &client::user;
    command_map["PRIVMSG"] = &client::privmsg;
    command_map["NOTICE"] = &client::privmsg;
    command_map["FILEMSG"] = &client::filemsg;
    command_map["QUIT"] = &client::quit;
    command_map["YES"] = &client::yes_no;
    command_map["NO"] = &client::yes_no;

    // command_map["JOIN"] = &client::join;
    // command_map["KICK"] = &client::kick;
    command_map["JOIN"] = &client::join;
    command_map["KICK"] = &client::kick;
    command_map["INVITE"] = &client::invite;
    command_map["TOPIC"] = &client::topic;
    command_map["MODE"] = &client::mode;
    command_map["PART"] = &client::part;
}

client::client(int fd, unsigned int port, const std::string &password) : clientManager(ClientManager::getInstance()), fd(fd), port(port), password(password), connected(false)
{

    confirme[0] = false;
    confirme[1] = false;
    std::stringstream fd1;
    fd1 << fd;
    this->nickname = "nick_" + fd1.str();
    this->count = false;
    command_map["CONNECT"] = &client::connect;
    command_map["NICK"] = &client::nick;
    command_map["USER"] = &client::user;
    command_map["PRIVMSG"] = &client::privmsg;
    command_map["NOTICE"] = &client::privmsg;
    command_map["FILEMSG"] = &client::filemsg;
    command_map["QUIT"] = &client::quit;
    command_map["YES"] = &client::yes_no;
    command_map["NO"] = &client::yes_no;

    command_map["JOIN"] = &client::join;
    command_map["KICK"] = &client::kick;
    command_map["INVITE"] = &client::invite;
    command_map["TOPIC"] = &client::topic;
    command_map["MODE"] = &client::mode;
    command_map["PART"] = &client::part;
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

void client::set_adress_ip(const std::string &adress_ip)
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

void client::set_confirme(int index, bool confirme)
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

std::string client::get_hostname()
{
    return this->hostname;
}

std::string client::server_hostname()
{
    std::string ret = "localhost";
    if (servername.empty())
    {
        return ret;
    }
    return this->servername;
}

//-----------------commands-----------------//

void client::check_cmd(int fd, const std::string &input)
{
    std::map<std::string, CommandHandler>::iterator it;
    for (it = command_map.begin(); it != command_map.end(); ++it)
    {
        if (input.find(it->first) != std::string::npos)
        {
            CommandHandler handler = it->second;
            (this->*handler)(fd, input);
            return;
        }
    }
    std::cerr << "Unknown command received from client " << fd << ": " << input << std::endl;
    dprintf(fd, "Error: unknown command\n");
}

void client::connect(int fd, const std::string &message)
{
    if (get_connected())
    {
        dprintf(fd, "Error: you are already connected\n");
        return;
    }

    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token)
    {
        args.push_back(token);
    }

    if (args.size() != 4 && !(args[0] == "CONNECT"))
    {
        dprintf(fd, "Error: invalid command or number of arguments\n");
        return;
    }

    if (get_connected())
    {
        dprintf(fd, "Error: you are already connected\n");
        return;
    }

    if (get_password() != args[3] || get_port() != args[2] || args[1] != "127.0.0.1")
    {
        dprintf(fd, "Error: invalid IP , password or port\n");
        return;
    }

    set_connected(true);
    dprintf(fd, "Connected\n");
}

void client::nick(int fd, const std::string &message)
{
    if (!get_connected())
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token)
    {
        args.push_back(token);
    }

    std::string currentNick = get_nickname();

    if (args.size() == 1 && (args[0] == "NICK"))
    {
        if (currentNick.empty())
        {
            dprintf(fd, "ERR_NONICKNAMEGIVEN\n");
        }
        else
        {
            dprintf(fd, "Your nickname is: %s\n", currentNick.c_str());
        }
    }
    else if (args.size() == 2 && (args[0] == "NICK"))
    {
        std::string newNick = args[1];
        if (!isValideNickname(fd, newNick))
            return;

        if (clientManager->nicknameUsed(newNick))
        {
            dprintf(fd, "ERR_NICKNAMEINUSE\n");
        }
        else
        {
            if (count)
                dprintf(fd, "use this format <currentNick> NICK <newNICK>\n");
            else
            {
                count = true;
                set_nickname(newNick);
                dprintf(fd, "Your nickname is: %s\n", newNick.c_str());
            }
        }
    }
    else if (args.size() == 3 && (args[1] == "NICK"))
    {
        std::string newNick = args[2];
        if (!isValideNickname(fd, newNick))
            return;

        if (currentNick.empty())
        {
            dprintf(fd, "ERR_NONICKNAMEGIVEN\n");
        }
        else if (args[0] != currentNick)
        {
            dprintf(fd, "ERR_NONMATCHNICKNAME\n");
        }
        else if (clientManager->nicknameUsed(newNick))
        {
            dprintf(fd, "ERR_NICKNAMEINUSE\n");
        }
        else
        {
            set_nickname(newNick);
            dprintf(fd, "Your nickname has been changed to: %s\n", newNick.c_str());
        }
    }
    else if (args.size() == 2 && args[1] == "NICK")
    {
        dprintf(fd, "ERR_NONICKNAMEGIVEN\n");
    }
    else
    {
        dprintf(fd, "Error: invalid command\n");
    }
}

void client::user(int fd, const std::string &message)
{
    if (!get_connected())
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token)
    {
        args.push_back(token);
    }

    if (args.size() != 5 && !(args[0] == "USER"))
    {
        dprintf(fd, "ERR_NEEDMOREPARAMS\n");
        return;
    }

    if (get_registered())
    {
        dprintf(fd, "ERR_ALREADYREGISTRED\n");
        return;
    }

    std::string username = args[1];
    std::string hostname = args[2];
    std::string servername = args[3];
    std::string realname = args[4];

    if (username.empty() || hostname.empty() || servername.empty() || realname.empty())
    {
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

void client::quit(int fd, const std::string &message)
{
    if (!get_connected())
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token)
    {
        args.push_back(token);
    }

    if (args.empty() && !(args[0] == "QUIT"))
    {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (args.size() == 1)
    {
        set_connected(false);
        dprintf(fd, "Disconnected from %s:%s\n", get_adress_ip().c_str(), get_port().c_str());
        dprintf(fd, "click a key  to exit terminal\n");
        clientManager->removeClient(fd);
        close(fd);
        return;
    }

    if (args.size() > 1)
    {
        size_t pos = message.find(":");
        if (pos != std::string::npos)
        {
            std::string reason = message.substr(pos + 1);
            dprintf(fd, "click to exit terminal\n");
            clientManager->broadcastToAll(get_nickname() + " has quit (" + reason + ")", fd);
            clientManager->removeClient(fd);
            return;
        }
    }

    dprintf(fd, "Error: invalid command\n");
}

void client::privmsg(int fd, const std::string &message)
{
    if (!get_connected())
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token)
    {
        args.push_back(token);
    }

    if (args.empty() || args[0] != "PRIVMSG")
    {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (args.size() < 3)
    {
        dprintf(fd, "Error: invalid number of arguments\n");
        return;
    }

    std::string recipient = args[1];

    size_t pos = message.find(":");
    if (pos == std::string::npos)
    {
        dprintf(fd, "Error: invalid message format\n");
        return;
    }

    std::string msg = message.substr(pos + 1);

    // Handling message to a channel : PRIVMSG #channelName :msg
    if (recipient[0] == '#')
    {
        Channel *channel = clientManager->getChannel(recipient);

        if (!channel)
        {
            dprintf(fd, "Error: no such channel\n");
            return;
        }
        if (!channel->isMember(this))
        {
            dprintf(fd, "Error: you are not on that channel\n");
            return;
        }

        channel->sendMessage(this, msg);
    }
    else
    {

        client *recipientClient = clientManager->getClient(recipient);

        if (recipientClient && recipientClient->get_fd() != fd)
        {
            dprintf(recipientClient->get_fd(), ":%s PRIVMSG %s :%s\n",
                    get_nickname().c_str(), recipient.c_str(), msg.c_str());
        }
        else
        {
            dprintf(fd, "Error: user not found\n");
        }
    }
}

void client::filemsg(int fd, const std::string &message)
{
    if (get_connected() == false)
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }
    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string recipient = count > 1 ? get_word(message, 1) : "";
    std::string filename = count > 2 ? get_word(message, 2) : "";

    if (!(command == "FILEMSG"))
    {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (count != 3)
    {
        dprintf(fd, "Error: invalid number of arguments\n");
        return;
    }

    std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open())
    {
        dprintf(fd, "Error: file not found\n");
        return;
    }

    client *recipientClient = clientManager->getClient(recipient);
    recipientClient->set_confirme(0, false);
    recipientClient->set_confirme(1, false);
    if (!recipientClient || recipientClient->get_fd() == fd)
    {
        dprintf(fd, "Error: user not found\n");
        return;
    }

    char buffer[4096];
    std::string file_contents;
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0)
    {
        file_contents.append(buffer, file.gcount());
    }

    recipientClient->set_save(file_contents);
    recipientClient->set_confirme(0, true);
    dprintf(recipientClient->get_fd(), "%s user has sent you a file named %s, do you want to accept it? (YES/NO)\n", get_nickname().c_str(), filename.c_str());

    file.close();
}

void client::yes_no(int fd, const std::string &message)
{
    if (get_connected() == false)
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }
    client *recipientClient = clientManager->getClient(fd);
    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";

    if (command != "YES" && command != "NO")
    {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (count != 1)
    {
        dprintf(fd, "Error: invalid number of arguments\n");
        return;
    }

    if (command == "YES" && recipientClient->get_confirme(0))
    {
        recipientClient->set_confirme(1, true);
        dprintf(fd, "File transfer accepted\n");
    }
    else if (command == "NO" && recipientClient->get_confirme(0))
    {
        dprintf(fd, "File transfer refused\n");
        recipientClient->set_confirme(0, false);
        recipientClient->set_confirme(1, false);
    }
    else
    {
        dprintf(fd, "Error: invalid response\n");
    }
    if (recipientClient->get_confirme(0) && recipientClient->get_confirme(1))
    {
        dprintf(fd, "%s", recipientClient->get_save().c_str());
        dprintf(fd, "\nfile transfer completed\n");
    }
    recipientClient->clear_save();
}
client::~client()
{
    set_connected(false);
}

// JOIN #channelName
void client::join(int fd, const std::string &message)
{
    if (!get_connected())
    {
        dprintf(fd, RED "Error: you are not connected\n" RESET);
        return;
    }

    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string channels = count > 1 ? get_word(message, 1) : "";
    std::string keys = count > 2 ? get_word(message, 2) : "";

    if (command != "JOIN")
    {
        dprintf(fd, RED "Error: invalid command\n" RESET);
        return;
    }
    if (channels.empty())
    {
        sendError(fd, "ERR_NEEDMOREPARAMS", "JOIN");
        return;
    }

    std::vector<std::string> channelList = split(channels, ',');
    std::vector<std::string> keyList = split(keys, ',');

    for (size_t i = 0; i < channelList.size(); ++i)
    {
        std::string channelName = channelList[i];
        std::string key = (i < keyList.size()) ? keyList[i] : "";

        if (!isValidChannelName(channelName))
        {
            sendError(fd, "ERR_NOSUCHCHANNEL", channelName);
            return;
        }

        Channel *channel = clientManager->getChannel(channelName);

        if (!channel)
        {
            std::cout << GREEN "Channel not found. Creating new channel: " << channelName << RESET << std::endl;
            clientManager->addChannel(channelName);
            channel = clientManager->getChannel(channelName);
            if (channel)
            {
                channel->addOperator(this);
                std::cout << GREEN "Channel created and operator added: " << channelName << RESET << std::endl;
            }
            else
            {
                std::cerr << RED "Failed to create channel: " << channelName << RESET << std::endl;
                sendError(fd, "ERR_CHANNELCREATIONFAILED", channelName);
                return;
            }
        }

        if (channel->isMember(this))
        {
            dprintf(fd, YELLOW "You are already in the channel %s\r\n" RESET, channelName.c_str());
            continue;
        }

        if (channel->hasMode("i") && !channel->isInvited(nickname) && !channel->isOperator(this))
        {
            sendError(fd, "ERR_INVITEONLYCHAN", channelName);
            return;
        }

        if (channel->isBanned(nickname))
        {
            sendError(fd, "ERR_BANNEDFROMCHAN", channelName);
            continue;
        }

        if (channel->hasMode("k"))
        {
            if (!channel->checkKey(key))
            {
                sendError(fd, "ERR_BADCHANNELKEY", channelName);
                return;
            }
        }

        if (channel->hasMode("l"))
        {
            int userLimit = channel->getUserLimit();
            std::cout << userLimit;
            std::cout << channel->getMemberCount();
            if (channel->getMemberCount() - 1 == userLimit)
            {
                sendError(fd, RED "ERR_CHANNELISFULL" RESET, channelName);
                return;
            }
        }

        channel->addMember(this);
        std::string joinMessage = CYAN ":" + get_nickname() + "!" + get_username() + "@" + get_hostname() + " JOIN " + channelName + "\r\n" RESET;
        std::vector<client *> memberList = channel->getMembers();
        for (std::vector<client *>::iterator it = memberList.begin(); it != memberList.end(); ++it)
        {
            client *member = *it; // Dereference the iterator to get the actual element
            dprintf(member->get_fd(), "%s", joinMessage.c_str());
        }

        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty())
        {
            dprintf(fd, MAGENTA ":%s 331 %s %s :No topic is set\r\n" RESET, server_hostname().c_str(), get_nickname().c_str(), channelName.c_str());
        }
        else
        {
            dprintf(fd, MAGENTA ":%s 332 %s %s :%s\r\n" RESET, server_hostname().c_str(), get_nickname().c_str(), channelName.c_str(), currentTopic.c_str());
        }

        std::string nameReply = "= " + channelName + " :";
        for (std::vector<client *>::iterator it = memberList.begin(); it != memberList.end(); ++it)
        {
            client *member = *it;
            nameReply += member->get_nickname() + " ";
        }
        nameReply += "\r\n";

        dprintf(fd, GREEN ":%s 353 %s  %s\r\n" RESET, server_hostname().c_str(), get_nickname().c_str(), nameReply.c_str());
        dprintf(fd, RED ":%s 366 %s %s :End of /NAMES list\r\n" RESET, server_hostname().c_str(), get_nickname().c_str(), channelName.c_str());
    }
}

// KICK #channelName member_nick
void client::kick(int fd, const std::string &message)
{
    if (!get_connected())
    {
        dprintf(fd, RED "Error: you are not connected\n" RESET);
        return;
    }

    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string channelName = count > 1 ? get_word(message, 1) : "";
    std::string userToKick = count > 2 ? get_word(message, 2) : "";
    std::string kickMessage = count > 3 ? get_word(message, 3) : "";

    if (command != "KICK")
    {
        dprintf(fd, RED "Error: invalid command\n" RESET);
        return;
    }
    if (channelName.empty() || userToKick.empty())
    {
        sendError(fd, "ERR_NEEDMOREPARAMS", "KICK");
        return;
    }

    if (!isValidChannelName(channelName))
    {
        sendError(fd, "ERR_NOSUCHCHANNEL", channelName);
        return;
    }

    Channel *channel = clientManager->getChannel(channelName);
    if (!channel)
    {
        sendError(fd, "ERR_NOSUCHCHANNEL", channelName);
        return;
    }

    if (!channel->isOperator(this))
    {
        sendError(fd, "ERR_CHANOPRIVSNEEDED", channelName);
        return;
    }

    client *target = clientManager->getClient(userToKick);
    if (!target)
    {
        sendError(fd, "ERR_NOSUCHNICK", userToKick);
        return;
    }

    if (!channel->isMember(target))
    {
        sendError(fd, "ERR_USERNOTINCHANNEL", userToKick);
        return;
    }
    if (!channel->isOperator(target))
    {
        channel->removeMember(target);
    }
    else
    {
        dprintf(fd, "%s", RED "Target is an operator\n" RESET);
        return;
    }
    std::string kickMessageFormatted = kickMessage.empty() ? "" : " :" + kickMessage;
    std::string kickNotification = ":" + get_nickname() + " KICK " + channelName + " " + userToKick + kickMessageFormatted + "\r\n";

    // Notify all members of the channel
    std::vector<client *> memberList = channel->getMembers();
    for (std::vector<client *>::iterator it = memberList.begin(); it != memberList.end(); ++it)
    {
        client *member = *it; // Dereference the iterator to get the actual client*
        dprintf(member->get_fd(), "%s", kickNotification.c_str());
    }

    // Notify the client who was kicked
    dprintf(target->get_fd(), "%s", kickNotification.c_str());
}

// INVITE nick #channelName
void client::invite(int fd, const std::string &message)
{
    if (!get_connected())
    {
        dprintf(fd, RED "Error: you are not connected\n" RESET);
        return;
    }

    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string userToInvite = count > 1 ? get_word(message, 1) : "";
    std::string channelName = count > 2 ? get_word(message, 2) : "";

    if (command != "INVITE")
    {
        dprintf(fd, RED "Error: invalid command\n" RESET);
        return;
    }

    if (userToInvite.empty() || channelName.empty())
    {
        sendError(fd, "ERR_NEEDMOREPARAMS", "INVITE");
        return;
    }

    if (!isValidChannelName(channelName))
    {
        sendError(fd, "ERR_NOSUCHCHANNEL", channelName);
        return;
    }

    Channel *channel = clientManager->getChannel(channelName);
    if (!channel)
    {
        sendError(fd, "ERR_NOSUCHCHANNEL", channelName);
        return;
    }

    client *target = clientManager->getClient(userToInvite);
    if (!target)
    {
        sendError(fd, "ERR_NOSUCHNICK", userToInvite);
        return;
    }

    if (channel->isMember(target))
    {
        sendError(fd, "ERR_USERONCHANNEL", userToInvite);
        return;
    }

    if (!channel->isOperator(this))
    {
        sendError(fd, "ERR_CHANOPRIVSNEEDED", channelName);
        return;
    }

    channel->inviteUser(userToInvite);
    std::string inviteMessage = ":" + get_nickname() + " INVITE " + userToInvite + " " + channelName + "\r\n";
    dprintf(target->get_fd(), "%s", inviteMessage.c_str());
    dprintf(fd, GREEN " Invitation sent\r\n" RESET);
}

// TOPIC #channelName {topic_string}
void client::topic(int fd, const std::string &message)
{
    if (!get_connected())
    {
        dprintf(fd, RED "Error: you are not connected\n" RESET);
        return;
    }

    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string channelName = count > 1 ? get_word(message, 1) : "";
    std::string topic = count > 2 ? message.substr(message.find(get_word(message, 2))) : "";

    if (!(command == "TOPIC"))
    {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (channelName.empty())
    {
        sendError(fd, "ERR_NEEDMOREPARAMS", "TOPIC");
        return;
    }

    Channel *channel = clientManager->getChannel(channelName);
    if (!channel)
    {
        sendError(fd, "ERR_NOSUCHCHANNEL", channelName);
        return;
    }

    if (!channel->isMember(this))
    {
        sendError(fd, "ERR_NOTONCHANNEL", channelName);
        return;
    }

    if (topic.empty())
    {
        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty())
        {
            dprintf(fd, RED "331 %s %s :No topic is set\r\n" RESET, get_nickname().c_str(), channelName.c_str());
        }
        else
        {
            dprintf(fd, CYAN "332 %s %s :%s\r\n" RESET, get_nickname().c_str(), channelName.c_str(), currentTopic.c_str());
        }
    }
    else
    {
        if (channel->hasMode("t") && !channel->isOperator(this))
        {
            sendError(fd, "ERR_CHANOPRIVSNEEDED", channelName);
            return;
        }

        channel->setTopic(topic);
        // Notify other users
        std::string topicMsg = GREEN ":" + get_nickname() + " TOPIC " + channelName + " :" + channel->getTopic() + "\r\n" RESET;
        std::vector<client *> memberList = channel->getMembers();
        for (std::vector<client *>::iterator it = memberList.begin(); it != memberList.end(); ++it)
        {
            client *member = *it; // Dereference the iterator to get the actual client*
            dprintf(member->get_fd(), "%s", topicMsg.c_str());
        }
    }
}

// MODE #channelName +/-mode nickname
void client::mode(int fd, const std::string &message)
{
    if (!get_connected())
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string channelName = count > 1 ? get_word(message, 1) : "";
    std::string modes = count > 2 ? get_word(message, 2) : "";
    std::string modeParam = count > 3 ? get_word(message, 3) : "";

    if (command != "MODE")
    {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (channelName.empty())
    {
        sendError(fd, "ERR_NEEDMOREPARAMS", "MODE");
        return;
    }

    Channel *channel = clientManager->getChannel(channelName);

    if (!channel)
    {
        sendError(fd, "ERR_NOSUCHCHANNEL", channelName);
        return;
    }

    if (!channel->isMember(this))
    {
        sendError(fd, "ERR_NOTONCHANNEL", channelName);
        return;
    }

    if (!channel->isOperator(this))
    {
        sendError(fd, "ERR_CHANOPRIVSNEEDED", channelName);
        return;
    }

    bool addMode = true;
    std::string appliedModes;
    std::string appliedParams;

    for (size_t i = 0; i < modes.size(); ++i)
    {
        char mode = modes[i];

        if (mode == '+')
        {
            addMode = true;
            continue;
        }
        else if (mode == '-')
        {
            addMode = false;
            continue;
        }

        switch (mode)
        {
        case 'i':
            if (addMode)
            {
                if (!channel->hasMode("i"))
                {
                    channel->addMode("i");
                    appliedModes += "+i";
                }
            }
            else
            {
                if (channel->hasMode("i"))
                {
                    channel->removeMode("i");
                    appliedModes += "-i";
                }
            }
            break;
        case 't':
            if (addMode)
            {
                if (!channel->hasMode("t"))
                {
                    channel->addMode("t");
                    appliedModes += "+t";
                }
            }
            else
            {
                if (channel->hasMode("t"))
                {
                    channel->removeMode("t");
                    appliedModes += "-t";
                }
            }
            break;

        case 'k':
            if (addMode)
            {
                if (!channel->hasMode("k"))
                { // Only set key if not already set
                    channel->setKey(modeParam);
                    channel->addMode("k");
                    appliedModes += "+k";
                    appliedParams += " " + modeParam;
                }
            }
            else
            {
                if (channel->hasMode("k"))
                { // Only remove key if it exists
                    channel->setKey("");
                    channel->removeMode("k");
                    appliedModes += "-k";
                }
            }
            break;

        case 'o':
            if (addMode)
            {
                client *newOp = channel->getMember(modeParam);
                channel->addOperator(newOp);
                appliedModes += "+o";
                appliedParams += " " + modeParam;
            }
            else
            {
                channel->removeOperator(this);
                appliedModes += "-o";
                appliedParams += " " + modeParam;
            }
            break;

        case 'l':
            if (addMode)
            {
                if (!channel->hasMode("l"))
                {
                    channel->setUserLimit(std::atoi(modeParam.c_str()));
                    channel->addMode("l");
                    appliedModes += "+l";
                    appliedParams += " " + modeParam;
                }
            }
            else
            {
                if (channel->hasMode("l"))
                {
                    channel->setUserLimit(-1);
                    channel->removeMode("l");
                    appliedModes += "-l";
                }
            }
            break;

        default:
            sendError(fd, "ERR_UNKNOWNMODE", std::string(1, mode));
            return;
        }
    }

    if (!appliedModes.empty())
    {
        std::string modeMessage = ":" + get_nickname() + " MODE " + channelName + " " + appliedModes + appliedParams + "\r\n";
        std::vector<client *> memberList = channel->getMembers();
        for (size_t i = 0; i < memberList.size(); ++i)
        {
            dprintf(memberList[i]->get_fd(), "%s", modeMessage.c_str());
        }
    }
}

// PART #channelName msg
void client::part(int fd, const std::string &message)
{
    if (!get_connected())
    {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string channelName = count > 1 ? get_word(message, 1) : "";
    std::string reason = count > 2 ? get_word(message, 2) : "";

    if (command != "PART")
    {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (channelName.empty())
    {
        sendError(fd, "ERR_NEEDMOREPARAMS", "PART");
        return;
    }

    Channel *channel = clientManager->getChannel(channelName);

    if (!channel)
    {
        sendError(fd, "ERR_NOSUCHCHANNEL", channelName);
        return;
    }

    if (!channel->isMember(this))
    {
        sendError(fd, "ERR_NOTONCHANNEL", channelName);
        return;
    }

    // Leave the channel
    channel->partChannel(this, reason);

    // Notify the client that they have left the channel
    dprintf(fd, ":%s PART %s :%s\r\n", get_nickname().c_str(), channelName.c_str(), reason.c_str());
}

/*

void client::mode(int fd, const std::string& message) {
   if (!get_connected()) {
        dprintf(fd, "Error: you are not connected\n");
        return;
    }

    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string target = count > 1 ? get_word(message, 1) : "";
    std::string mode = count > 2 ? get_word(message, 2) : "";
    std::string param = count > 3 ? get_word(message, 3) : "";

    if (command != "MODE") {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (target.empty()) {
        sendError(fd, "ERR_NEEDMOREPARAMS", "MODE");
        return;
    }

    Channel* channel = clientManager->getChannel(target);

    if (!channel) {
        sendError(fd, "ERR_NOSUCHCHANNEL", target);
        return;
    }

    if (mode.empty()) {
        dprintf(fd, "Error: mode required\n");
        return;
    }

    // Set the channel mode
    channel->changeMode(mode, param);

    // Notify other users
    std::string modeReply = "MODE " + target + " " + mode + (param.empty() ? "" : " " + param);
    std::vector<client*> memberList = channel->getMembers();
    for (client* member : memberList) {
        dprintf(member->get_fd(), "%s\r\n", modeReply.c_str());
    }

    dprintf(fd, GREEN "Mode set\r\n" RESET);
}



























void client::mode(int fd, const std::string& message) {
     if (!get_connected()) {
        dprintf(fd, RED "Error: you are not connected\n" RESET);
        return;
    }

    int count = count_words(message);
    std::string command = count > 0 ? get_word(message, 0) : "";
    std::string channelName = count > 1 ? get_word(message, 1) : "";
    std::string modes = count > 2 ? get_word(message, 2) : "";
    std::string modeParam = count > 3 ? get_word(message, 3) : "";

    if (command != "MODE") {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    if (channelName.empty()) {
        sendError(fd, "ERR_NEEDMOREPARAMS", "MODE");
        return;
    }

    Channel* channel = clientManager->getChannel(channelName);

    if (!channel) {
        std::cerr << "Channel not found:" << channelName << std::endl;
        sendError(fd, "ERR_NOSUCHCHANNEL", channelName);
        return;
    }

    if (!channel->isMember(this)) {
        sendError(fd, "ERR_NOTONCHANNEL", channelName);
        return;
    }

    if (!channel->isOperator(get_nickname())) {
        sendError(fd, "ERR_CHANOPRIVSNEEDED", channelName);
        return;
    }

    bool addMode = true;
    for (size_t i = 0; i < modes.size(); ++i) {
        char mode = modes[i];

        if (mode == '+' || mode == '-') {
            addMode = (mode == '+');
            continue;
        }

        switch (mode) {
            case 'i':
                if (addMode) {
                    channel->addMode("i");
                } else {
                    channel->removeMode("i");
                }
                break;
            case 't':
                if (addMode) {
                    channel->addMode("t");
                } else {
                    channel->removeMode("t");
                }
                break;
            case 'k':
                if (addMode) {
                    channel->setKey(modeParam);
                } else {
                    channel->setKey("");
                }
                break;
            case 'o':
                if (addMode) {
                    channel->addOperator(modeParam);
                } else {
                    channel->removeOperator(modeParam);
                }
                break;
            case 'l':
                if (addMode) {
                    channel->setUserLimit(std::atoi(modeParam.c_str()));
                } else {
                    channel->setUserLimit(-1);
                }
                break;
            default:
                sendError(fd, "ERR_UNKNOWNMODE", std::string(1, mode));
                return;
        }
    }

    std::string modeMessage = ":" + get_nickname() + " MODE " + channelName + " " + modes + " " + modeParam + "\r\n";
    std::vector<client*> memberList = channel->getMembers();
    for (size_t i = 0; i < memberList.size(); ++i) {
        dprintf(memberList[i]->get_fd(), "%s", modeMessage.c_str());
    }
}
*/