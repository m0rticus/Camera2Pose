from poseSocket import Client as socketClient

def main():
    print("Listening...")
    socket_client = socketClient.CommunicationSocket()
    while True:
        socket_client.recvMessage()

if __name__ == "__main__":
    main()
