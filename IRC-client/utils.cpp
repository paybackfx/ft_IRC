/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anas <anas@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 21:43:48 by afennoun          #+#    #+#             */
/*   Updated: 2024/06/18 00:52:22 by anas             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "./IRC/server.hpp"
      
int count_words(const std::string& str) {
        std::istringstream iss(str);
        std::vector<std::string> words;
        std::string word;
        while (iss >> word) {
            words.push_back(word);
        }
        return words.size();
    }

std::string get_word(const std::string& str, int index) {
        std::istringstream iss(str);
        std::vector<std::string> words;
        std::string word;
        while (iss >> word) {
            words.push_back(word);
        }
        if (index >= 0 && index < words.size()) {
            return words[index];
        }
        return "";
    }

bool isValideNickname(int fd, std::string nickname)
{
    if (nickname.length() > 9)
        return false;
    for (int i = 0; i < nickname.length(); i++)
    {
        if (!std::isalnum(nickname[i])){
            
            dprintf(fd, "ERR_ERRONEUSNICKNAME\n");
            return false;
        }
    }
    return true;
}

