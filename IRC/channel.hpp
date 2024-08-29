// channel.hpp
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <map>
#include <vector>

class client;
class ClientManager;
class Channel {
private:
    std::string name;
    std::string topic;
    std::string key; // Add a key attribute
    std::map<int, client*> members;
    std::set<std::string> modes; 
    std::set<std::string> bannedUsers;
    std::set<std::string> invitedUsers;
    std::set<std::string> operators;
    int limit;

public:
    Channel(const std::string& name);
    ~Channel();

    // Channel operations
    void addMember(client* client);
    void removeMember(client* client);
    bool isMember(client* client) const;

    // Mode operations
    void addMode(const std::string& mode);
    void removeMode(const std::string& mode);
    bool hasMode(const std::string& mode) const;
    //void changeMode(const std::string& mode, const std::string& param = "");

    // Operator operations
    void addOperator(const std::string& nickname);
    void removeOperator(const std::string& nickname);
    bool isOperator(const std::string& nickname) const;

    // Topic operations
    void setTopic(const std::string& topic);
    std::string getTopic() const;

    // Ban/Invite operations
    void banUser(const std::string& nickname);
    void unbanUser(const std::string& nickname);
    bool isBanned(const std::string& nickname) const;

    void inviteUser(const std::string& nickname);
    bool isInvited(const std::string& nickname) const;

    // User limit operations
    void setUserLimit(int limit);
    int getUserLimit() const;
    bool hasUserLimit() const;

    // Key operations
    void setKey(const std::string& key);
    bool checkKey(const std::string& key) const;

    // Getters
    std::string getName() const;
    std::vector<client*> getMembers() const;
    int getMemberCount() const;

    // Operator commands
    void kickMember(client* client, const std::string& reason);
    void changeTopic(client* client, const std::string& newTopic);
    void changeMode(client* client, const std::string& mode, bool enable);

    // Logging
    void logMessage(const std::string& message);
};

#endif
