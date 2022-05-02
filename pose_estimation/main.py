import cv2
import mediapipe as mp
import time
from threading import Thread
from poseSocket import Client as socketClient

###############################
wCam, hCam = 1280, 720
###############################

class PoseDetector:

    def __init__(self, mode=False, upBody=False, smooth=True, detectCon=0.5, trackCon=0.95):
        self.mode = mode
        self.upBody = upBody
        self.smooth = smooth
        self.detectCon = detectCon
        self.trackCon = trackCon

        self.mpDraw = mp.solutions.drawing_utils
        self.mpPose = mp.solutions.pose
        self.pose = self.mpPose.Pose(self.mode, 1, self.smooth, False, False, self.detectCon, self.trackCon)

    def findPose(self, img, draw=True):
        img.flags.writeable = False
        imgRGB = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        # process the poses and values
        self.results = self.pose.process(imgRGB)
        # if the landmark exists, connect the dots
        if self.results.pose_landmarks:
            if draw:
                self.mpDraw.draw_landmarks(img, self.results.pose_landmarks, self.mpPose.POSE_CONNECTIONS)
        return img

    def findPosition(self, img, draw=True):
        lmList = []
        if self.results.pose_world_landmarks:
            # get precise x and y coordinates of each landmark
            for markId, landmark in enumerate(self.results.pose_world_landmarks.landmark):
                h, w, c = img.shape
                # print(markId, landmark)
                # cx, cy, cz = int(landmark.x * w), int(landmark.y * h), int(landmark.z * c)
                cx, cy, cz = round(landmark.x, 6), round(landmark.y, 6), round(landmark.z, 6)
                lmList.append([cx, cy, cz])
                # cv2.circle(img, (cx, cy), 10, (255, 0, 125), cv2.FILLED)
        return lmList

class ThreadedCamera(object):
    def __init__(self, source = 0):

        self.capture = cv2.VideoCapture(source, cv2.CAP_DSHOW)
        self.capture.set(3, wCam)
        self.capture.set(4, hCam)
        self.capture.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))

        self.thread = Thread(target = self.update, args = ())
        self.thread.daemon = True
        self.thread.start()
        
        self.thread2 = Thread(target = self.update, args = ())
        self.thread2.daemon = True
        self.thread2.start()

        self.status = False
        self.frame = None
        self.colorFrame = None

    def update(self):
        while True:
            if self.capture.isOpened():
                (self.status, self.frame) = self.capture.read()

    def grab_frame(self):
        if self.status:
            return self.frame
        return None  

def main():
    # initialize camera
    cap = ThreadedCamera()
    # cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
    # cap.set(3, wCam)
    # cap.set(4, hCam)
    # cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
    prevTime = 0
    detector = PoseDetector()
    socket_client = socketClient.CommunicationSocket()

    while True:
        # read in the image
        img = cap.grab_frame()
        if img is not None:
            # success, img = cap.read()
            img = detector.findPose(img)

            # find a specific landmark and highlight
            lmList = detector.findPosition(img)

            if len(lmList) >= 32:
                # print("x is" + str(lmList[13][0]))
                # print("y is" + str(lmList[13][1]))
                # print("z is" + str(lmList[13][2]))
                # lmList.append(offsetList)
                socket_client.send_landmark_data(lmList)

            # calculate fps
            currTime = time.time()
            fps = 1 / (currTime - prevTime)
            prevTime = currTime

            print(fps)
            # update frame
            cv2.putText(img, f'FPS: {int(fps)}', (40, 70), cv2.FONT_HERSHEY_SIMPLEX, 3, (0, 255, 255), 3)
            cv2.imshow("Frame", img)
        cv2.waitKey(1)

    socket_client.destroy()


if __name__ == "__main__":
    main()
