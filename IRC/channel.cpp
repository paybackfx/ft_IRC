#include "channel.hpp"
#include "../client.hpp"
#include <cstddef>
#include <cstdio>
#include <string>
#include <vector>
#include <unistd.h>

Channel::Channel(const std::string &name) : name(name), limit(-1) {}

Channel::~Channel() {}


 void Channel::setOwner(client *newOwner) {
        owner = newOwner;
        addOperator(owner); 
 }

bool Channel::isOwner(client *member) const {
        return owner == member;
}

void Channel::addMember(client *client)
{
    members[client->get_fd()] = client;
    memberOrder.push_back(client);
}

void Channel::removeMember(client *client)
{
    members.erase(client->get_fd());
    memberOrder.remove(client);
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

 void Channel::addOperator(client *member) {
        operators.insert(member);
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


void Channel::partChannel(client *sender, const std::string &reason)
{
    if (!isMember(sender))
    {
        return;
    }

    bool wasOperator = isOperator(sender);

    removeMember(sender);
    std::string partMessage = ":" + sender->get_nickname() + " PART " + name + " :" + reason + "\r\n";
    broadcast(partMessage, sender->get_fd());
}

void Channel::sendMessage(client *sender, const std::string &message) {
    if (!isMember(sender)) {
        return;
    }

    // Format the message according to RFC 1459
    std::string formattedMessage = ":" + sender->get_nickname() + " PRIVMSG " + name + " :" + message + "\r\n";

    // Use broadcast to send the message to all members, excluding the sender
    broadcast(formattedMessage, sender->get_fd());
}

void Channel::broadcast(const std::string &formattedMessage, int excludeFd) {
 
    std::map<int, client *>::iterator it;
    for (it = members.begin(); it != members.end(); ++it) {
        client *member = it->second;
        
        // Skip the sender if excludeFd is provided
        if (member->get_fd() != excludeFd) {
            dprintf(member->get_fd(), "%s", formattedMessage.c_str());
        }
    }
}


std::vector<client*> Channel::getEligibleMembers() const {
    std::vector<client*> eligibleMembers;

    for (std::list<client*>::const_iterator it = memberOrder.begin(); it != memberOrder.end(); ++it) {
        client *member = *it;
        // we can add sophistical criteria for eligibility here
        if (!isOperator(member) && removedOperators.find(member) == removedOperators.end()) {
            eligibleMembers.push_back(member);
        }
    }
    return eligibleMembers;
}

void Channel::appointNewOperator(int excludeFd) {
    // Prevent appointing the same operator repeatedly in quick succession
    if (!operators.empty()) {
        return; // If there's already an operator, don't appoint again
    }

    std::vector<client *> eligibleMembers = getEligibleMembers();
    if (eligibleMembers.empty()) {
        // No eligible members to appoint as a new operator
        return;
    }

    // Choose the first eligible member as the new operator
    client *newOperator = eligibleMembers[0];
    addOperator(newOperator);

    // Notify members about the new operator appointment
    std::string message = newOperator->get_nickname() + " has been appointed as the new operator.\n";
    broadcast(message, excludeFd);
}

void Channel::handleOperatorQuit(client *quittingOperator) {

    // Ensure we only handle the operator quitting once
    if (!isOperator(quittingOperator)) {
        return;
    }

    // Remove the quitting operator
    removeOperator(quittingOperator);
    // Mark them as excluded from future appointments
    removedOperators.insert(quittingOperator);

    // Notify all members about the operator's departure
    std::string quitMessage = quittingOperator->get_nickname() + " was an operator in " + getName() + "\n";
    broadcast(quitMessage, quittingOperator->get_fd());

    // Check if there are no operators left in the channel
    if (operators.empty()) {
        // Try to appoint a new operator
        appointNewOperator(quittingOperator->get_fd()); 
    } else {
        client *nextOperator = *operators.begin();
        std::string newOpMessage = nextOperator->get_nickname() + " has been appointed as the new operator.\n";
        broadcast(newOpMessage, quittingOperator->get_fd());
    }
}