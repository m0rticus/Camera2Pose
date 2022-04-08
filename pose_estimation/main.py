import cv2
import mediapipe as mp
import time
# import socket.Client as socketClient

###############################
wCam, hCam = 1280, 720
###############################

class PoseDetector:

    def __init__(self, mode=True, upBody=False, smooth=True, detectCon=0.8, trackCon=0.95):
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
        if self.results.pose_landmarks:
            # get precise x and y coordinates of each landmark
            for markId, landmark in enumerate(self.results.pose_landmarks.landmark):
                h, w, c = img.shape
                # print(markId, landmark)
                cx, cy, cz = int(landmark.x * w), int(landmark.y * h), int(landmark.z * c)
                lmList.append([markId, cx, cy, cz])
                # cv2.circle(img, (cx, cy), 10, (255, 0, 125), cv2.FILLED)
        return lmList

def main():
    # initialize camera
    cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
    cap.set(3, wCam)
    cap.set(4, hCam)
    prevTime = 0
    detector = PoseDetector()
    # socket_client = socketClient.CommunicationSocket()

    while True:
        # read in the image
        success, img = cap.read()
        img = detector.findPose(img)

        # find a specific landmark and highlight
        lmList = detector.findPosition(img)
        # if len(lmList) >= 29:
        #     print(lmList[27])
        #     cv2.circle(img, (lmList[28][1], lmList[28][2]), 15, (255, 255, 0), cv2.FILLED)
        #     print(lmList[28])
        #     cv2.circle(img, (lmList[27][1], lmList[27][2]), 15, (255, 255, 0), cv2.FILLED)

        # calculate fps
        currTime = time.time()
        fps = 1 / (currTime - prevTime)
        prevTime = currTime

        # update frame
        cv2.putText(img, f'FPS: {int(fps)}', (40, 70), cv2.FONT_HERSHEY_SIMPLEX, 3, (0, 255, 255), 3)
        cv2.imshow("Frame", img)
        cv2.waitKey(1)


if __name__ == "__main__":
    main()
