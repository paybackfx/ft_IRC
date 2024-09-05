/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clientM.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afennoun <afennoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 18:48:45 by afennoun          #+#    #+#             */
/*   Updated: 2024/08/29 14:20:29 by afennoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clientM.hpp"
#include "client.hpp"
#include <unistd.h>
#include <cstdio>
#include <sys/epoll.h>


std::map<int, client*> ClientManager::connectedClients;
std::map<std::string, Channel*> ClientManager::channels;
ClientManager* ClientManager::instance = NULL;

int fd_epoll = 0;
ClientManager* ClientManager::getInstance() {
    if (!instance) {
        instance = new ClientManager();
    }
    return instance;
}

void ClientManager::destroyInstance() {
    if (instance) {
        delete instance;
        instance = NULL;
    }
}

void ClientManager::addClient(int clientFd, client* newClient) {
        
        dprintf(clientFd, "Welcome to the IRC server %d\n", clientFd);
        connectedClients[clientFd] = newClient;
    }

void ClientManager::removeClient(int clientFd) {
        
         delete connectedClients[clientFd]; 
        connectedClients.erase(clientFd);
        epoll_ctl(fd_epoll, EPOLL_CTL_DEL, clientFd, NULL);
        close(clientFd);
    }
    //CONNECT 127.0.0.1 8888 8888 

client* ClientManager::getClient(int clientFd) {

        if (connectedClients.find(clientFd) != connectedClients.end()) {
            return connectedClients[clientFd];
        }
        return NULL;
    }
client* ClientManager::getClient(std::string nickname) {
        for (std::map<int, client*>::iterator it = connectedClients.begin(); it != connectedClients.end(); it++) {
            if (it->second->get_nickname() == nickname) {
                return it->second;
            }
        }
        return NULL;
}
bool ClientManager::isClientConnected(int clientFd) {
        return connectedClients.find(clientFd) != connectedClients.end();
    }

bool ClientManager::isClientConnected(std::string nickname) {
        for (std::map<int, client*>::iterator it = connectedClients.begin(); it != connectedClients.end(); it++) {
            if (it->second->get_nickname() == nickname) {
                return true;
            }
        }
    return false;
}
    
bool ClientManager::nicknameUsed(std::string nickname) {
        for (std::map<int, client*>::iterator it = connectedClients.begin(); it != connectedClients.end(); it++) {
            if (it->second->get_nickname() == nickname) {
                
                // dprintf(it->first, "ERR_NICKNAMEINUSE\n");
                return true;
            }
        }
    return false;
}

bool ClientManager::isClient(int clientFd) {
        return connectedClients.find(clientFd) != connectedClients.end();
}

bool ClientManager::isClient(std::string nickname) {
        for (std::map<int, client*>::iterator it = connectedClients.begin(); it != connectedClients.end(); it++) {
            if (it->second->get_nickname() == nickname) {
                return true;
            }
        }
    return false;
}

bool ClientManager::isClientByUsername(std::string username) {
        for (std::map<int, client*>::iterator it = connectedClients.begin(); it != connectedClients.end(); it++) {
            if (it->second->get_username() == username) {
                return true;
            }
        }
    return false;
}

void ClientManager::broadcastToAll(const std::string& message, int excludedClientFd )
    {
        for (std::map<int, client*>::iterator it = connectedClients.begin(); it != connectedClients.end(); it++) {
            if (it->first != excludedClientFd && it->second->get_connected() )
            {
                dprintf(it->first, "%s\n", message.c_str());
            }
        }
    }


//Channel management 
void ClientManager::addChannel(const std::string& name) {
    std::string lowerName = toLowerCase(name);
    if (!isChannel(lowerName)) {
        channels[lowerName] = new Channel(lowerName);
    }
}


void ClientManager::removeChannel(const std::string& name) {
    if (isChannel(name)) {
        delete channels[name];
        channels.erase(name);
    }
}

Channel* ClientManager::getChannel(const std::string& channelName) {
    std::string lowerChannelName = toLowerCase(channelName);
    std::cout << "getChannel called with: " << lowerChannelName << std::endl;
    std::cout << "Channel map size: " << channels.size() << std::endl;

    if (channels.find(lowerChannelName) != channels.end()) {
        std::cout << "Channel found: " << lowerChannelName << std::endl;
        return channels[lowerChannelName];
    } else {
        std::cout << "Channel not found: " << lowerChannelName << std::endl;
        return NULL;
    }
}


bool ClientManager::isChannel(const std::string& name) {
    return channels.find(name) != channels.end();
}

void ClientManager::free_all() {
    for (std::map<int, client*>::iterator it = connectedClients.begin(); it != connectedClients.end(); it++) {
        delete it->second;
    }
    connectedClients.clear();

    for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); it++) {
        delete it->second;
    }
    channels.clear();
    
    delete instance; // Supprimer l'instance de ClientManager
}