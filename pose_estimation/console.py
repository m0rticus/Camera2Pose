from poseSocket import Client as socketClient

def main():
    socket_client = socketClient.CommunicationSocket(port=5006)
    # print("Sending stuff...")
    # socket_client.sendMessage("1.2345")
    while True:
        print("Listening forever...")
        print(socket_client.recvMessage())
        socket_client.sendMessage("that's right we got it")

if __name__ == "__main__":
    main()
