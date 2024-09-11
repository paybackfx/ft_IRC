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

std::string server_hostname = "localhost";

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
    command_map["BOT"] = &client::bot;
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

    if (args.size() != 4 || !(args[0] == "CONNECT"))
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
    // if (!get_connected())
    // {
    //     dprintf(fd, "Error: you are not connected\n");
    //     return;
    // }

    // Split the message into parts
    std::vector<std::string> args;
    std::istringstream iss(message);
    std::string token;
    while (iss >> token)
    {
        args.push_back(token);
    }

    // Check for the valid QUIT command
    if (args.empty() || args[0] != "QUIT")
    {
        dprintf(fd, "Error: invalid command\n");
        return;
    }

    // Handle case where there's no reason provided
    if (args.size() == 1)
    {
        set_connected(false);
        dprintf(fd, "Disconnected from %s:%s\n", get_adress_ip().c_str(), get_port().c_str());
        dprintf(fd, "Press any key to exit terminal\n");

        // Remove client from all channels
        clientManager->removeClientFromChannels(fd);

        // Remove client and close connection
        clientManager->removeClient(fd);
        close(fd);
        return;
    }

    // Handle case where a reason for quitting is provided
    if (args.size() > 1)
    {
        size_t pos = message.find(":");
        if (pos != std::string::npos)
        {
            std::string reason = message.substr(pos + 1);
            dprintf(fd, "Press any key to exit terminal\n");

            // Broadcast the quit message to all channels
            clientManager->broadcastToAll(get_nickname() + " has quit (" + reason + ") \n", fd);

            // Remove client from all channels
            clientManager->removeClientFromChannels(fd);

            // Remove client fron clientManager and close connection
            clientManager->removeClient(fd);
            close(fd);
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
///////////////////////// IRC channel commands /////////////////////////

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
            // Create a new channel and assign the current client as the owner
            clientManager->addChannel(channelName);
            channel = clientManager->getChannel(channelName);
            if (channel)
            {
                channel->setOwner(this);
                std::cout << GREEN "Channel created and owner added: " << channelName << RESET << std::endl;
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

        // Check invite-only mode
        if (channel->hasMode("i") && !channel->isInvited(nickname) && !channel->isOperator(this) && !channel->isOwner(this))
        {
            sendError(fd, "ERR_INVITEONLYCHAN", channelName);
            return;
        }

        // Check if user is banned
        if (channel->isBanned(nickname))
        {
            sendError(fd, "ERR_BANNEDFROMCHAN", channelName);
            continue;
        }

        // Check key requirement
        if (channel->hasMode("k"))
        {
            if (!channel->checkKey(key))
            {
                sendError(fd, "ERR_BADCHANNELKEY", channelName);
                return;
            }
        }

        // Check user limit
        if (channel->hasMode("l"))
        {
            int userLimit = channel->getUserLimit();
            if (channel->getMemberCount() == userLimit)
            {
                sendError(fd, "ERR_CHANNELISFULL", channelName);
                return;
            }
        }

        // Add the client as a member to the channel
        channel->addMember(this);
        std::string joinMessage = CYAN ":" + get_nickname() + "!" + get_username() + "@" + get_hostname() + " JOIN " + channelName + "\r\n" RESET;

        // Notify all channel members of the new join
        std::vector<client *> memberList = channel->getMembers();
        for (std::vector<client *>::iterator it = memberList.begin(); it != memberList.end(); ++it)
        {
            client *member = *it;
            dprintf(member->get_fd(), "%s", joinMessage.c_str());
        }

        // Show the topic to the joining client
        std::string currentTopic = channel->getTopic();
        if (currentTopic.empty())
        {
            dprintf(fd, MAGENTA ":%s 331 %s %s :No topic is set\r\n" RESET, server_hostname().c_str(), get_nickname().c_str(), channelName.c_str());
        }
        else
        {
            dprintf(fd, MAGENTA ":%s 332 %s %s :%s\r\n" RESET, server_hostname().c_str(), get_nickname().c_str(), channelName.c_str(), currentTopic.c_str());
        }

        // List the current members in the channel
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

    // Check if the client is neither an operator nor the owner
    if (!channel->isOperator(this) && !channel->isOwner(this))
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

    // Owner has permission to kick anyone, including other operators
    if (channel->isOwner(this))
    {
        channel->removeMember(target);
    }

    // Regular operators cannot kick the owner or other operators
    else if (channel->isOperator(this))
    {
        if (channel->isOwner(target))
        {
            dprintf(fd, RED "Error: you cannot kick the owner\n" RESET);
            return;
        }
        else if (channel->isOperator(target))
        {
            dprintf(fd, RED "Error: you cannot kick another operator\n" RESET);
            return;
        }
        else
        {
            channel->removeMember(target);
        }
    }
    else
    {
        dprintf(fd, RED "Error: insufficient privileges\n" RESET);
        return;
    }

    std::string kickMessageFormatted = kickMessage.empty() ? "" : " :" + kickMessage;
    std::string kickNotification = ":" + get_nickname() + " KICK " + channelName + " " + userToKick + kickMessageFormatted + "\r\n";

    // Notify all members of the channel
    channel->broadcast(kickNotification, fd);

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
        channel->broadcast(topicMsg, fd);
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
                if (modeParam.empty())
                {
                    sendError(fd, "ERR_NEEDMOREPARAMS", "MODE +k");
                    return;
                }
                channel->setKey(modeParam); // Always update the key
                if (!channel->hasMode("k"))
                {
                    channel->addMode("k");
                }
                appliedModes += "+k";
                appliedParams += " " + modeParam;
            }
            else
            {
                if (channel->hasMode("k"))
                {
                    channel->setKey(""); // Remove the key when mode is removed
                    channel->removeMode("k");
                    appliedModes += "-k";
                }
            }
            break;

        case 'o':
            if (addMode)
            {
                if (modeParam.empty())
                {
                    sendError(fd, "ERR_NEEDMOREPARAMS", "MODE +o");
                    return;
                }
                client *newOp = channel->getMember(modeParam);
                if (newOp)
                {
                    channel->addOperator(newOp);
                    appliedModes += "+o";
                    appliedParams += " " + modeParam;
                }
                else
                {
                    sendError(fd, "ERR_USERNOTINCHANNEL", modeParam);
                }
            }
            else
            {
                if (modeParam.empty())
                {
                    sendError(fd, "ERR_NEEDMOREPARAMS", "MODE -o");
                    return;
                }
                client *removeOp = channel->getMember(modeParam);
                if (removeOp)
                {
                    channel->removeOperator(removeOp);
                    appliedModes += "-o";
                    appliedParams += " " + modeParam;
                }
                else
                {
                    sendError(fd, "ERR_USERNOTINCHANNEL", modeParam);
                }
            }
            break;

        case 'l':
            if (addMode)
            {
                if (modeParam.empty())
                {
                    sendError(fd, "ERR_NEEDMOREPARAMS", "MODE +l");
                    return;
                }
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
        channel->broadcast(modeMessage, fd);
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

void client::bot(int fd, const std::string &message)
{
    if (!get_connected())
    {
        std::vector<std::string> args;
        args.push_back("to connect to the server");
        args.push_back(std::string("use") + GREEN + " CONNECT <IP> <PORT> <PASSWORD>" RESET);
        args.push_back("-----------------------------");
        args.push_back("to leave the server");
        args.push_back(std::string("use") + GREEN + " QUIT  {<:>} {<reason>}" RESET);
        printCenteredBox(fd, args);
        return;
    }

    if (clientManager->isInChannels(fd))
    {
        if ((clientManager->isOperatorInChannel(fd)))
        {
            std::vector<std::string> args;
            args.push_back("to invite a user to a channel");
            args.push_back(std::string("use") + GREEN + " INVITE <nick> <#channelName>" RESET);
            args.push_back("-----------------------------");
            args.push_back("to kick a user from a channel");
            args.push_back(std::string("use") + GREEN + " KICK <#channelName> <nick> {<reason>}" RESET);
            args.push_back("-----------------------------");
            args.push_back("to change the mode of a channel");
            args.push_back(std::string("use") + GREEN + " MODE <#channelName> {+|-}<mode> {<nick>}" RESET);
            args.push_back("-----------------------------");
            args.push_back("to leave a channel");
            args.push_back(std::string("use") + GREEN + " PART <#channelName> {<reason>}" RESET);
            args.push_back("-----------------------------");
            args.push_back("to send a message to a channel or a user in channel");
            args.push_back(std::string("use") + GREEN + " PRIVMSG <#channelName> {<targetName>} :<message>" RESET);
            args.push_back("-----------------------------");
            args.push_back("to get or change name of  the topic of a channel");
            args.push_back(std::string("use") + GREEN + " TOPIC <#channelName> {<topic_string>}" RESET);
            args.push_back("-----------------------------");
            printCenteredBox(fd, args);
        }
        else
        {
            std::vector<std::string> args;
            args.push_back("to send a message to a channel or a user in channel");
            args.push_back(std::string("use") + GREEN + " PRIVMSG <#channelName> {<targetName>} :<message>" RESET);
            args.push_back("-----------------------------");
            args.push_back("to get or change name of  the topic of a channel");
            args.push_back(std::string("use") + GREEN + " TOPIC <#channelName> {<topic_string>}" RESET);
            args.push_back("-----------------------------");
            args.push_back("to leave a channel");
            args.push_back(std::string("use") + GREEN + " PART <#channelName> {<reason>}" RESET);
            printCenteredBox(fd, args);
        }

    }
    else if (get_connected() && !clientManager->isInChannels(fd))
        {
            std::vector<std::string> args; 
            args.push_back("for channel commands");
            args.push_back("to join a channel");
            args.push_back(std::string("use") + GREEN + " JOIN <#channelName> {<key>}" RESET);
            args.push_back("-----------------------------");
            args.push_back("to leave a channel");
            args.push_back(std::string("use") + GREEN + " PART <#channelName> {<reason>}" RESET);
            args.push_back(RED "-----------------------------" RESET);
            args.push_back("for server management");
            args.push_back("to choose a nickname or get the current one ");
            args.push_back(", max 9 alphnumeric characters");
            args.push_back(std::string("use") + RED + " NICK {<newNick>}" RESET);
            args.push_back("-----------------------------");
            args.push_back("to register a user");
            args.push_back(std::string("use") + RED + " USER <username> <hostname> <servername> <realname>" RESET);
            args.push_back("-----------------------------");
            args.push_back("to send a message to a user ");
            args.push_back(std::string("use") + RED + " PRIVMSG <nick> <:> <message>" RESET);
            args.push_back("-----------------------------");
            args.push_back("to send a file to a user ");
            args.push_back(std::string("use") + RED + " FILEMSG <nick> <filename>" RESET);
            args.push_back("-----------------------------");
            args.push_back("to send a notification to a user ");
            args.push_back(std::string("use") + RED + " NOTICE <nick> <:> <message>" RESET);
            args.push_back("-----------------------------");
            args.push_back("to quit the server");
            args.push_back(std::string("use") + RED + " QUIT {<:>} {<reason>} or <CRT + C >" RESET);
            printCenteredBox(fd, args);
        }
}