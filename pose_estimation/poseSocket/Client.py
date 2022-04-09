import socket

class CommunicationSocket:

	def __init__(self, host = "127.0.0.1", port = 5005):
		self.host = host # as both code is running on same pc
		self.port = port  # socket server port number

		self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # instantiate
		self.client_socket.connect((host, port))  # connect to the server
		
	def sendMessage(self, message):
		self.client_socket.send(message.encode())
		
	def destroy(self):
		self.client_socket.close()
		
def client_program():
    client_socket = CommunicationSocket()
    message = input(' ->')
	
    while message.lower().strip() != 'bye':
        client_socket.sendMessage(message)  # send message
        message = input(" -> ")  # again take input

    client_socket.destroy()  # close the connection


if __name__ == '__main__':
    client_program()