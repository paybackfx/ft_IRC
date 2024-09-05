# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: anas <anas@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/15 10:47:44 by nbaghoug          #+#    #+#              #
#    Updated: 2024/06/25 22:45:30 by anas             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXXFLAGS = -std=c++98  -fsanitize=address -g3
CXX = c++
SRCS = ./IRC/ircserv.cpp ./IRC/server.cpp client.cpp clientM.cpp utils.cpp ./IRC/channel.cpp
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
