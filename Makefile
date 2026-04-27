NAME		= ircserv
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98

INC_DIR		= inc
SRC_DIR		= src
CMD_DIR		= $(SRC_DIR)/commands
OBJ_DIR		= obj

SRCS		= $(SRC_DIR)/main.cpp \
			  $(SRC_DIR)/Server.cpp \
			  $(SRC_DIR)/Client.cpp \
			  $(SRC_DIR)/Channel.cpp \
			  $(SRC_DIR)/CommandDispatcher.cpp \
			  $(CMD_DIR)/Pass.cpp \
			  $(CMD_DIR)/Nick.cpp \
			  $(CMD_DIR)/User.cpp \
			  $(CMD_DIR)/Join.cpp \
			  $(CMD_DIR)/Part.cpp \
			  $(CMD_DIR)/Quit.cpp \
			  $(CMD_DIR)/Privmsg.cpp \
			  $(CMD_DIR)/Kick.cpp \
			  $(CMD_DIR)/Invite.cpp \
			  $(CMD_DIR)/Topic.cpp \
			  $(CMD_DIR)/Mode.cpp

OBJS		= $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
