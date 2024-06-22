# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marvin <marvin@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/15 10:47:44 by nbaghoug          #+#    #+#              #
#    Updated: 2024/06/22 22:58:04 by marvin           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -fsanitize=address -g3
CXX = c++
SRCS = ./IRC/ircserv.cpp ./IRC/server.cpp client.cpp clientM.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o) 

NAME = ircserv

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re  
