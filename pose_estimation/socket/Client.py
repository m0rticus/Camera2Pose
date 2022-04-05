import socket, time

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
    message = input('-->')
	
    while message.lower().strip() != 'bye':
        client_socket.sendMessage(message)  # send message
        message = input(" -> ")  # again take input

    client_socket.destroy()  # close the connection


# takes as argument a two dimensional list containing landmarks and their corresponding x, y and z coordinates
# each coordinate for each landmark is sent to the server
# all numbers are separated by space characters
# lmList[0, ..., 32][x, y, z] -> x0 y0 z0 ... x32 y32 z32

def intake_landmark(lmList):
	client_socket = CommunicationSocket()
	message = ' '.join([(' '.join([str(coordinate) for coordinate in landmark])) for landmark in lmList]) # convert two dimensional list to a string
	client_socket.sendMessage(message)
	client_socket.destroy() # close the connection

	
if __name__ == '__main__':
	# use list to test operation of intake_landmark function
	test_list = [[82359.32 for x in range(2)] for y in range(32)]
	intake_landmark(test_list)