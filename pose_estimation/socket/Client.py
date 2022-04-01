import socket
import time

def client_program():
    host = "127.0.0.1"  # as both code is running on same pc
    port = 5005  # socket server port number

    client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # instantiate
    client_socket.connect((host, port))  # connect to the server

    message = input(" -> ")  # take input

    while message.lower().strip() != 'bye':
        start = time.time_ns()/1000000
        
        client_socket.send(message.encode())  # send message
        data = client_socket.recv(1024).decode()  # receive response
        end = time.time_ns()/1000000
        print('Received from server: ' + data)  # show in terminal
        t = (end-start)
        print("Time it takes: "+ str(t))
        message = input(" -> ")  # again take input

    client_socket.close()  # close the connection


if __name__ == '__main__':
    client_program()