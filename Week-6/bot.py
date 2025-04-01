#!/usr/bin/env python3

from base64 import b16encode
import socket
import sys

address = sys.argv[1]

s = socket.socket()
s.connect((address, 8008))

MSG_WELCOME = 0
MSG_MAZE_DATA = 1
MSG_ILLEGAL_MOVE = 5
MSG_NOT_YOUR_TURN = 6
MSG_PLAYER_POS = 7
MSG_PLAYER_TURN = 8
MSG_PLAYER_JOINED = 10
MSG_PLAYER_WINS = 12
MSG_NEW_GAME = 13

MAZE_SYMBOLS = {
	'11': [1, 0, 1, 1],  # Left wall only
	'12': [0, 1, 1, 1],  # Left wall only
	'13': [0, 0, 1, 1],  # Left and Bottom walls
	'14': [1, 1, 1, 0],  # Right wall only
	'15': [1, 0, 1, 0],  # Left and Right walls
	'16': [0, 1, 1, 0],  # Right and Bottom walls
	'17': [0, 0, 1, 0],  # Right, Bottom, and Left walls
	'18': [1, 1, 0, 1],  # Top and Left walls
	'19': [1, 0, 0, 1],  # Top and Left walls
	'1A': [0, 1, 0, 1],  # Top and Bottom walls
	'1B': [0, 0, 0, 1],  # Top, Bottom, and Left walls
	'1C': [1, 1, 0, 0],  # Top and Right walls
	'1D': [1, 0, 0, 0],  # Top, Right, and Left walls
	'1E': [0, 1, 0, 0],  # Top, Right, and Bottom walls
}

MOVE_JUMP_DOWN = 22
MOVE_WALK_DOWN = 18
MOVE_JUMP_RIGHT = 19
MOVE_WALK_RIGHT = 15
MOVE_WALK_LEFT = 17

class Player:
    def __init__(self):
        self.active = False
        self.col = -1
        self.row = -1
        self.jumps_remaining = 3  # Initialize jump counter
        self.position_tracker = {}

class MazeGame:
    def __init__(self):
        self.cells = None
        self.col_count = 0
        self.row_count = 0
    
    def get_cell(self, row, col):
        if 0 <= row < self.row_count and 0 <= col < self.col_count:
            cell_value = b16encode(self.cells[self.col_count * row + col:][:1]).decode()
            return MAZE_SYMBOLS.get(cell_value, [0, 0, 0, 0])
        return [1, 1, 1, 1]  # Treat out of bounds as walls
    
    def has_bottom_wall(self, row, col):
        cell = self.get_cell(row, col)
        return cell[2] == 1
    
    def has_right_wall(self, row, col):
        cell = self.get_cell(row, col)
        return cell[1] == 1

maze = MazeGame()
players = []
my_player_id = -1
max_players = -1
visited = set()

def to_bytes(i):
    return i.to_bytes(byteorder='little')

def to_int(b):
    return int.from_bytes(b, byteorder='little')

def recv(n):
    return to_int(s.recv(n, socket.MSG_WAITALL))

def send_move():
	my_player = players[my_player_id]
	current_row, current_col = my_player.row, my_player.col
	
	if current_col != maze.col_count - 1:
		if maze.has_right_wall(current_row, current_col):
			s.send(to_bytes(MOVE_JUMP_RIGHT))
		else:
			s.send(to_bytes(MOVE_WALK_RIGHT))
	else:
		if maze.has_bottom_wall(current_row, current_col):
			s.send(to_bytes(MOVE_JUMP_DOWN))
		else:
			s.send(to_bytes(MOVE_WALK_DOWN))
    
while True:
    msg_type = recv(1)
    if msg_type == MSG_WELCOME:
        my_player_id = recv(1)
        max_players = recv(1)
        print(f"Welcome player {my_player_id + 1} of {max_players}")
        for i in range(max_players):
            players.append(Player())
        players[my_player_id].active = True
    
    elif msg_type == MSG_MAZE_DATA:
        size = recv(2)
        maze.col_count = recv(1)
        maze.row_count = recv(1)
        maze.cells = s.recv(maze.col_count * maze.row_count, socket.MSG_WAITALL)
        print(f"Received maze ({maze.col_count} x {maze.row_count}):")
        for row in range(0, maze.row_count):
            for col in range(0, maze.col_count):
                print(b16encode(maze.cells[maze.col_count*row+col:][:1]), end=" ")
            print(" ")
    
    elif msg_type == MSG_ILLEGAL_MOVE:
        print("Illegal Move!!")
    
    elif msg_type == MSG_NOT_YOUR_TURN:
        print("It is not your turn!")
    
    elif msg_type == MSG_PLAYER_POS:
        player_id = recv(1)
        p = players[player_id]
        p.col = recv(1)
        p.row = recv(1)
        print(f"Player {player_id+1} moved to ({p.col+1}, {p.row+1})")
    
    elif msg_type == MSG_PLAYER_TURN:
        player_id = recv(1)
        print(f"It is player {player_id+1}'s turn")
        if player_id == my_player_id:
            print("My turn !!")
            send_move()
    
    elif msg_type == MSG_PLAYER_JOINED:
        player_id = recv(1)
        print(f"Player {player_id + 1} joined")
        players[player_id].active = True
    
    elif msg_type == MSG_PLAYER_WINS:
        player_id = recv(1)
        print(f"Player {player_id+1} won!")
        if player_id == my_player_id:
            print("I win!")
    
    elif msg_type == MSG_NEW_GAME:
        print("Starting a new game!")
        # Reset jumps for new game
        for player in players:
            player.jumps_remaining = 3
    
    else:
        print(f"Unrecognized message: {msg_type}")
