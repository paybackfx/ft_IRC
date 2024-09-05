#include "channel.hpp"
#include "../client.hpp"
#include <cstddef>
#include <cstdio>
#include <string>
#include <vector>
#include <unistd.h>

Channel::Channel(const std::string &name) : name(name), limit(-1) {}

Channel::~Channel() {}

void Channel::addMember(client *client)
{
    members[client->get_fd()] = client;
}

void Channel::removeMember(client *client)
{
    members.erase(client->get_fd());
}

bool Channel::isMember(client *client) const
{
    return members.find(client->get_fd()) != members.end();
}

void Channel::addMode(const std::string &mode)
{
    if (!mode.empty())
    {
        modes.insert(mode);
    }
}

void Channel::removeMode(const std::string &mode)
{
    if (!mode.empty())
    {
        modes.erase(mode);
    }
}

bool Channel::hasMode(const std::string &mode) const
{
    return modes.find(mode) != modes.end();
}

void Channel::setTopic(const std::string &topic)
{
    this->topic = topic;
}

std::string Channel::getTopic() const
{
    return topic;
}

void Channel::banUser(const std::string &nickname)
{
    bannedUsers.insert(nickname);
}

void Channel::unbanUser(const std::string &nickname)
{
    bannedUsers.erase(nickname);
}

bool Channel::isBanned(const std::string &nickname) const
{
    return bannedUsers.find(nickname) != bannedUsers.end();
}

void Channel::inviteUser(const std::string &nickname)
{
    invitedUsers.insert(nickname);
}

bool Channel::isInvited(const std::string &nickname) const
{
    return invitedUsers.find(nickname) != invitedUsers.end();
}

void Channel::setKey(const std::string &key)
{
    this->key = key;
}

bool Channel::checkKey(const std::string &key) const
{
    return this->key == key;
}

void Channel::addOperator(client *client)
{
    operators.insert(client);
}

void Channel::removeOperator(client *client)
{
    operators.erase(client);
}

bool Channel::isOperator(client *client) const
{
    return operators.find(client) != operators.end();
}

void Channel::setUserLimit(int limit)
{
    this->limit = limit;
}

int Channel::getUserLimit() const
{
    return this->limit;
}

client *Channel::getMember(const std::string &nickname) const
{
    std::vector<client *> members = getMembers();
    std::vector<client *>::const_iterator it;

    for (it = members.begin(); it != members.end(); ++it)
    {
        if ((*it)->get_nickname() == nickname)
        {
            return *it;
        }
    }
    return NULL;
}

bool Channel::hasUserLimit() const
{
    return limit > 0;
}

std::string Channel::getName() const
{
    return name;
}

std::vector<client *> Channel::getMembers() const
{
    std::vector<client *> memberList;
    for (std::map<int, client *>::const_iterator it = members.begin(); it != members.end(); ++it)
    {
        memberList.push_back(it->second);
    }
    return memberList;
}

int Channel::getMemberCount() const
{
    return members.size();
}

void Channel::kickMember(client *client, const std::string &reason)
{
    if (isMember(client))
    {
        removeMember(client);
        std::string kickMessage = ":" + client->get_nickname() + " KICK " + name + " :" + reason + "\r\n";
        logMessage(kickMessage);
    }
}

void Channel::changeTopic(client *client, const std::string &newTopic)
{
    setTopic(newTopic);

    // Notify other members about the topic change
    std::string topicMessage = ":" + client->get_nickname() + " TOPIC " + getName() + " :" + newTopic + "\r\n";
    logMessage(topicMessage);
}

void Channel::logMessage(const std::string &message)
{
    std::vector<client *> memberList = getMembers();
    for (size_t i = 0; i < memberList.size(); ++i)
    {
        dprintf(memberList[i]->get_fd(), "%s", message.c_str());
    }
}

void Channel::partChannel(client *client, const std::string &reason)
{
    if (!isMember(client))
    {
        return;
    }

    bool wasOperator = isOperator(client);

    removeMember(client);
    std::string partMessage = ":" + client->get_nickname() + " PART " + name + " :" + reason + "\r\n";
    logMessage(partMessage);
}

void Channel::sendMessage(client *sender, const std::string &message)
{

    if (!isMember(sender))
    {
        return;
    }

    // Format the message according to RFC 1459: ":<nickname> PRIVMSG <channel> :<message>"
    std::string formattedMessage = ":" + sender->get_nickname() + " PRIVMSG " + name + " :" + message + "\r\n";

    std::map<int, client *>::iterator it;
    for (it = members.begin(); it != members.end(); ++it)
    {
        client *member = it->second;
        dprintf(member->get_fd(), "%s", formattedMessage.c_str());
    }
}