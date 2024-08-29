// channel.cpp
#include "channel.hpp"
#include "../client.hpp"
#include <cstddef>
#include <cstdio>
#include <string>
#include <vector>
#include <unistd.h>

Channel::Channel(const std::string& name) : name(name), limit(-1) {}

Channel::~Channel() {}

void Channel::addMember(client* client) {
    members[client->get_fd()] = client;
}

void Channel::removeMember(client* client) {
    members.erase(client->get_fd());
}

bool Channel::isMember(client* client) const {
    return members.find(client->get_fd()) != members.end();
}

void Channel::addMode(const std::string& mode) {
    if (!mode.empty()) {
        modes.insert(mode);
    }
}

// Removes the mode(s) from the channel's mode set
void Channel::removeMode(const std::string& mode) {
    if (!mode.empty()) {
        modes.erase(mode);
    }
}

// Checks if the channel has a specific mode
bool Channel::hasMode(const std::string& mode) const {
    return modes.find(mode) != modes.end();
}


void Channel::setTopic(const std::string& topic) {
    this->topic = topic;
}

std::string Channel::getTopic() const {
    return topic;
}

void Channel::banUser(const std::string& nickname) {
    bannedUsers.insert(nickname);
}

void Channel::unbanUser(const std::string& nickname) {
    bannedUsers.erase(nickname);
}

bool Channel::isBanned(const std::string& nickname) const {
    return bannedUsers.find(nickname) != bannedUsers.end();
}

void Channel::inviteUser(const std::string& nickname) {
    invitedUsers.insert(nickname);
}

bool Channel::isInvited(const std::string& nickname) const {
    return invitedUsers.find(nickname) != invitedUsers.end();
}

std::string Channel::getName() const {
    return name;
}

std::vector<client*> Channel::getMembers() const {
    std::vector<client*> memberList;
    for (std::map<int, client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
        memberList.push_back(it->second);
    }
    return memberList;
}

void Channel::setKey(const std::string& key) {
    this->key = key;
}

bool Channel::checkKey(const std::string& key) const {
    return this->key == key;
}

void Channel::addOperator(const std::string& nickname) {
    operators.insert(nickname);
}

void Channel::removeOperator(const std::string& nickname) {
    operators.erase(nickname);
}

bool Channel::isOperator(const std::string& nickname) const {
    return operators.find(nickname) != operators.end();
}

void Channel::setUserLimit(int limit) {
    this->limit = limit;
}

int Channel::getUserLimit() const {
    return this->limit;
}

bool Channel::hasUserLimit() const {
    return limit > 0;
}

int Channel::getMemberCount() const {
    return members.size();
}

void Channel::kickMember(client* client, const std::string& reason) {
    if (isMember(client)) {
        removeMember(client);
        std::string kickMessage = ":" + client->get_nickname() + " KICK " + name + " :" + reason + "\r\n";
        logMessage(kickMessage);
    }
}

void Channel::changeTopic(client* client, const std::string& newTopic) {
    setTopic(newTopic);

    // Notify other members about the topic change
    std::string topicMessage = ":" + client->get_nickname() + " TOPIC " + getName() + " :" + newTopic + "\r\n";
    logMessage(topicMessage);
}

void Channel::logMessage(const std::string& message) {
    std::vector<client*> memberList = getMembers();
    for (size_t i = 0; i < memberList.size(); ++i) {
        dprintf(memberList[i]->get_fd(), "%s", message.c_str());
    }
}








/*


void Channel::changeMode(const std::string& mode, const std::string& param) {
    if (mode.empty()) return;

    std::string updatedModes = modes;
    if (mode[0] == '+') {
        for (char m : mode.substr(1)) {
            switch (m) {
                case 'i':  // Invite-only
                    if (updatedModes.find(" +i") == std::string::npos) {
                        updatedModes += " +i";
                    }
                    break;
                case 'k':  // Key
                    if (!param.empty()) {
                        key = param;
                        if (updatedModes.find(" +k") == std::string::npos) {
                            updatedModes += " +k";
                        }
                    } else {
                        std::cerr << "Error: Key required for mode +k" << std::endl;
                    }
                    break;
                case 'l':  // Limit
                    if (!param.empty()) {
                        limit = std::stoi(param);
                        if (updatedModes.find(" +l") == std::string::npos) {
                            updatedModes += " +l";
                        }
                    } else {
                        std::cerr << "Error: Limit required for mode +l" << std::endl;
                    }
                    break;
                case 'm':  // Moderated
                    if (updatedModes.find(" +m") == std::string::npos) {
                        updatedModes += " +m";
                    }
                    break;
                case 'n':  // No external messages
                    if (updatedModes.find(" +n") == std::string::npos) {
                        updatedModes += " +n";
                    }
                    break;
                case 't':  // Topic change restricted to operators
                    if (updatedModes.find(" +t") == std::string::npos) {
                        updatedModes += " +t";
                    }
                    break;
                case 's':  // Secret
                    if (updatedModes.find(" +s") == std::string::npos) {
                        updatedModes += " +s";
                    }
                    break;
                case 'p':  // Private
                    if (updatedModes.find(" +p") == std::string::npos) {
                        updatedModes += " +p";
                    }
                    break;
                default:
                    std::cerr << "Error: Unknown mode character " << m << std::endl;
                    break;
            }
        }
    } else if (mode[0] == '-') {
        for (char m : mode.substr(1)) {
            switch (m) {
                case 'i':
                    updatedModes.erase(updatedModes.find(" +i"), 3);
                    break;
                case 'k':
                    key.clear();
                    updatedModes.erase(updatedModes.find(" +k"), 3);
                    break;
                case 'l':
                    limit = 0;
                    updatedModes.erase(updatedModes.find(" +l"), 3);
                    break;
                case 'm':
                    updatedModes.erase(updatedModes.find(" +m"), 3);
                    break;
                case 'n':
                    updatedModes.erase(updatedModes.find(" +n"), 3);
                    break;
                case 't':
                    updatedModes.erase(updatedModes.find(" +t"), 3);
                    break;
                case 's':
                    updatedModes.erase(updatedModes.find(" +s"), 3);
                    break;
                case 'p':
                    updatedModes.erase(updatedModes.find(" +p"), 3);
                    break;
                default:
                    std::cerr << "Error: Unknown mode character " << m << std::endl;
                    break;
            }
        }
    }

    modes = updatedModes;
    //sendModeChangeToMembers("MODE " + modes + (key.empty() ? "" : " " + key) + (limit ? " " + std::to_string(limit) : ""));
}

*/