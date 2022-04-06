#  Camera2Pose (WIP)

Not solely my work. Based off of Terminal29's repository.

## Requirements

Python 3.9.x

Visual Studio 2019 (Tested on Community Edition)

OpenCV-Python (pip install opencv-contrib-python)

Mediapipe (pip install mediapipe)

A camera for pose estimation.

## Development Set up
- To create a virtual environment type run `python -m venv C2P-env` 
- To activate the environment run 
    - Windows: `C2P-env\Scripts\activate.bat`
    - Unix/MacOs:  `source tutorial-env/bin/activate`
- To install all the python packages run `python -m pip install -r req.txt`





## Tutorial

To begin the project:

Open Visual Studio 2019. Clone the repository. Type "cmake ." and right click the solution file and click build. Navigate to the Debug folder and copy the example folder.

From there, navigate to SteamVR drivers folder. Paste the example driver here.

Run main.py in pose_estimation folder. This process will remain running to read pose data from your camera.


