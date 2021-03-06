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

	def send_landmark_data(self, lmList):
		message = ' '.join([(' '.join([str(coordinate) for coordinate in landmark])) for landmark in lmList]) # convert two dimensional list to a string
		self.sendMessage(message)
	
	def recvMessage(self):
		data = self.client_socket.recv(1024).decode()
		return data
		
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
# def send_landmark_data(lmList):
# 	client_socket = CommunicationSocket()
# 	message = ' '.join([(' '.join([str(coordinate) for coordinate in landmark])) for landmark in lmList]) # convert two dimensional list to a string
# 	client_socket.sendMessage(message)
# 	client_socket.destroy() # close the connection
	
if __name__ == '__main__':
	# use list to test operation of send_landmark function
	test_list = [[1 for x in range(3)] for y in range(33)]
	print(len(test_list))
	print(test_list[20])
	send_landmark_data(test_list)