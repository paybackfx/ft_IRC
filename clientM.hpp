/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clientM.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anas <anas@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 18:47:47 by afennoun          #+#    #+#             */
/*   Updated: 2024/06/19 02:34:28 by anas             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTM_HPP
#define CLIENTM_HPP

#include <map>
#include <string>

class client; // Déclaration avant utilisation de la classe client

class ClientManager {
private:
    
    static ClientManager* instance;
    static std::map<int, client*> connectedClients; // Stocker des pointeurs vers des clients
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
};

#endif