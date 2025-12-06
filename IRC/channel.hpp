#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <list>
#include <string>
#include <set>
#include <map>
#include <vector>

class client;
class ClientManager;

class Channel
{
private:
    std::string name;
    std::string topic;
    std::string key;
    client *owner;
    std::map<int, client *> members;
    std::set<std::string> modes;
    std::set<std::string> bannedUsers;
    std::set<std::string> invitedUsers;
    std::set<client *> operators;
    std::set<client*> removedOperators; 
    std::list<client*> memberOrder;
    int limit;

public:
    Channel(const std::string &name);
    ~Channel();

    void setOwner(client *newOwner);
    bool isOwner(client *member) const;

    void addMember(client *client);
    void removeMember(client *client);
    bool isMember(client *client) const;

    void addMode(const std::string &mode);
    void removeMode(const std::string &mode);
    bool hasMode(const std::string &mode) const;

    void addOperator(client *client);      // Updated
    void removeOperator(client *client);   // Updated
    bool isOperator(client *client) const; // Updated

    void setTopic(const std::string &topic);
    std::string getTopic() const;

    void banUser(const std::string &nickname);
    void unbanUser(const std::string &nickname);
    bool isBanned(const std::string &nickname) const;

    void inviteUser(const std::string &nickname);
    bool isInvited(const std::string &nickname) const;

    void setUserLimit(int limit);
    int getUserLimit() const;
    bool hasUserLimit() const;

    void setKey(const std::string &key);
    bool checkKey(const std::string &key) const;

    std::string getName() const;
    std::vector<client *> getMembers() const;
    int getMemberCount() const;
    client *getMember(const std::string &nickname) const;

    /*
        `void kickMember(client *client, const std::string &reason);
        void changeTopic(client *client, const std::string &newTopic);
        void changeMode(client *client, const std::string &mode, bool enable);
    */

    void partChannel(client *client, const std::string &reason);
    void sendMessage(client *sender, const std::string &message);
    void broadcast(const std::string &message, int senderFd = -1);

    void handleOperatorQuit(client *quittingOperator);
    void appointNewOperator(int excludeFd = -1);
    std::vector<client*> getEligibleMembers() const;
};

#endif
