/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clientM.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anas <anas@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 18:48:45 by afennoun          #+#    #+#             */
/*   Updated: 2024/06/21 02:03:48 by anas             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "clientM.hpp"
#include "client.hpp"
#include <unistd.h>
#include <cstdio>

std::map<int, client*> ClientManager::connectedClients;
ClientManager* ClientManager::instance = NULL;
// Définition de la méthode getInstance
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
        
        dprintf(clientFd, "Welcome to the IRC server\n %d\n", clientFd);
        connectedClients[clientFd] = newClient;
    }

void ClientManager::removeClient(int clientFd) {
        connectedClients.erase(clientFd);
    }

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