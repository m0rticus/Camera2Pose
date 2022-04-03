import socket

class CommunicationSocket:

	def __init__():
		host = "127.0.0.1"  # as both code is running on same pc
		port = 5005  # socket server port number

		self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # instantiate
		self.client_socket.connect((host, port))  # connect to the server
		
	def sendMessage(self, message):
		self.client_socket.send(message.encode())
		
	def destroy(self):
		self.client_socket.close()
		
def client_program():
    host = "127.0.0.1"  # as both code is running on same pc
    port = 5005  # socket server port number

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # instantiate
    client_socket.connect((host, port))  # connect to the server

    message = input(" -> ")  # take input

    while message.lower().strip() != 'bye':
        client_socket.send(message.encode())  # send message
        data = client_socket.recv(1024).decode()  # receive response

        print('Received from server: ' + data)  # show in terminal

        message = input(" -> ")  # again take input

    client_socket.close()  # close the connection


if __name__ == '__main__':
    client_program()