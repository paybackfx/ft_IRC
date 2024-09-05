/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clientM.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afennoun <afennoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 18:47:47 by afennoun          #+#    #+#             */
/*   Updated: 2024/08/29 14:16:08 by afennoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTM_HPP
#define CLIENTM_HPP

#include <map>
#include <string>

#include "./IRC/channel.hpp"
extern int fd_epoll;
class client; // Déclaration avant utilisation de la classe client
class  Channel;
class ClientManager {
private:
    
    static ClientManager* instance;
    static std::map<int, client*> connectedClients; // Stocker des pointeurs vers des clients
    static std::map<std::string, Channel*> channels; // Map of channels

public:
    ClientManager() {}
    static ClientManager* getInstance();
    static void destroyInstance();
    void addClient(int clientFd, client* newClient); // Prend un pointeur vers un client
    void removeClient(int clientFd);
    client* getClient(int clientFd); // Retourne un pointeur vers un client
    client* getClient(std::string nickname);
    bool isClientConnected(int clientFd);
    bool isClientConnected(std::string nickname);
    bool nicknameUsed(std::string nickname);
    bool isClient(int clientFd);
    bool isClient(std::string nickname);
    bool isClientByUsername(std::string username);
    void broadcastToAll(const std::string& message, int excludedClientFd );
    void free_all();


     // Channel management
    void addChannel(const std::string& name);
    void removeChannel(const std::string& name);
    Channel* getChannel(const std::string& channelName);
    bool isChannel(const std::string& name);
};

#endif