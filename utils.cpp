/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afennoun <afennoun@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 21:43:48 by afennoun          #+#    #+#             */
/*   Updated: 2024/09/05 16:14:40 by afennoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "./IRC/server.hpp"

int count_words(const std::string &str)
{
    std::istringstream iss(str);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word)
    {
        words.push_back(word);
    }
    return words.size();
}

std::string get_word(const std::string &str, unsigned int index)
{
    std::istringstream iss(str);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word)
    {
        words.push_back(word);
    }
    if (index < words.size())
    {
        return words[index];
    }
    return "";
}

bool isValideNickname(int fd, std::string &nickname)
{
    if (nickname.length() > 9)
    {
        dprintf(fd, "ERR_ERRONEUSNICKNAME\n");
        return false;
    }
    for (unsigned int i = 0; i < nickname.length(); i++)
    {
        if (!std::isalnum(nickname[i]))
        {

            dprintf(fd, "ERR_ERRONEUSNICKNAME\n");
            return false;
        }
    }
    return true;
}

bool isValidChannelName(const std::string &channelName)
{
    if (channelName.empty() || channelName.size() > 200)
    {
        return false;
    }

    // Channel names must start with either '#' or '&'
    if ((channelName[0] != '#' && channelName[0] != '&') || channelName.size() < 2)
    {
        return false;
    }

    // Check for invalid characters: spaces, bell characters, and commas
    if (channelName.find(' ') != std::string::npos ||
        channelName.find('\x07') != std::string::npos || // Bell character
        channelName.find(',') != std::string::npos)
    {
        return false;
    }

    return true;
}

std::vector<std::string> split(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

void sendError(int fd, const std::string &errorCode, const std::string &channelName)
{
    std::string errorMessage;

    if (errorCode == "ERR_NOSUCHCHANNEL")
    {
        errorMessage = "403 " + channelName + " :No such channel\r\n";
    }
    else if (errorCode == "ERR_INVITEONLYCHAN")
    {
        errorMessage = "473 " + channelName + " :Cannot join channel (invite only)\r\n";
    }
    else if (errorCode == "ERR_BANNEDFROMCHAN")
    {
        errorMessage = "474 " + channelName + " :Cannot join channel (banned)\r\n";
    }
    else if (errorCode == "ERR_BADCHANNELKEY")
    {
        errorMessage = "475 " + channelName + " :Cannot join channel (bad key)\r\n";
    }
    else if (errorCode == "ERR_NEEDMOREPARAMS")
    {
        errorMessage = "461 " + channelName + " :Not enough parameters\r\n";
    }
    else if (errorCode == "ERR_CHANOPRIVSNEEDED")
    {
        errorMessage = "482 " + channelName + " :You're not channel operator\r\n";
    }
    else if (errorCode == "ERR_USERNOTINCHANNEL")
    {
        errorMessage = "441 " + channelName + " :They aren't on that channel\r\n";
    }
    else if (errorCode == "ERR_CHANNELISFULL")
    {
        errorMessage = "471" + channelName + " : the channel is full\r\n";
    }

    // Send the error message to the client using dprintf
    dprintf(fd, "%s", errorMessage.c_str());
}

std::string trim(const std::string &str)
{
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

std::string toLowerCase(const std::string &str)
{
    std::string lowerStr = str;
    for (std::string::size_type i = 0; i < lowerStr.size(); ++i)
    {
        lowerStr[i] = std::tolower(static_cast<unsigned char>(lowerStr[i]));
    }
    return lowerStr;
}

std::string stripAnsiCodes(const std::string& text) {
    std::string result;
    bool inEscapeSequence = false;
    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
        char ch = *it;
        if (ch == '\033') {
            inEscapeSequence = true;
        } else if (ch == 'm' && inEscapeSequence) {
            inEscapeSequence = false;
        } else if (!inEscapeSequence) {
            result += ch;
        }
    }
    return result;
}

void printCenteredBox(int fd, const std::vector<std::string>& lines) {
    // Déterminer la largeur maximale des lignes sans les séquences d'échappement
    size_t maxLength = 0;
    for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
        std::string strippedLine = stripAnsiCodes(*it);
        if (strippedLine.size() > maxLength) {
            maxLength = strippedLine.size();
        }
    }
    
    // La largeur totale de la boîte est la longueur maximale + 4 pour les bords et les espaces
    size_t boxWidth = maxLength + 4;

    // Afficher le haut de la boîte
    dprintf(fd, "+%s+\n", std::string(boxWidth - 2, '-').c_str());

    // Afficher les lignes du texte
    for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
        std::string strippedLine = stripAnsiCodes(*it);
        size_t padding = (boxWidth - 2 - strippedLine.size()) / 2;
        dprintf(fd, "|%*s%s%*s|\n",
                static_cast<int>(padding), "",
                it->c_str(), // Affiche le texte avec les séquences de couleur
                static_cast<int>(boxWidth - 2 - strippedLine.size() - padding), "");
    }
    
    // Afficher le bas de la boîte
    dprintf(fd, "+%s+\n", std::string(boxWidth - 2, '-').c_str());
}

