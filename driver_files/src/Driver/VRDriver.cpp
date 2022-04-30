#include "VRDriver.hpp"
#include <Driver/HMDDevice.hpp>
#include <Driver/TrackerDevice.hpp>
#include <Driver/Server.hpp>
#include <openvr_driver.h>
#include <cmath>   

#define MAXLINE 1024

vr::EVRInitError ExampleDriver::VRDriver::Init(vr::IVRDriverContext* pDriverContext)
{
    // Perform driver context initialisation
    if (vr::EVRInitError init_error = vr::InitServerDriverContext(pDriverContext); init_error != vr::EVRInitError::VRInitError_None) {
        return init_error;
    }

    Log("Activating ExampleDriver...");

    // Add a HMD
    // this->AddDevice(std::make_shared<HMDDevice>("Example_HMDDevice"));

    // Add a tracker
    this->AddDevice(std::make_shared<TrackerDevice>("Jason"));
    this->AddDevice(std::make_shared<TrackerDevice>("Emory"));
    this->AddDevice(std::make_shared<TrackerDevice>("Matthew"));
    this->AddDevice(std::make_shared<TrackerDevice>("Chris"));
    this->AddDevice(std::make_shared<TrackerDevice>("Alexis"));

    socketServer = new PoseSocketServer(5005);

    Log("ExampleDriver Loaded Successfully");

	return vr::VRInitError_None;
}

void ExampleDriver::VRDriver::Cleanup()
{
    delete socketServer;
}

void ExampleDriver::VRDriver::RunFrame()
{
    // Collect events
    vr::VREvent_t event;
    std::vector<vr::VREvent_t> events;
    while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event)))
    {
        events.push_back(event);
    }
    this->openvr_events_ = events;

    // Collect server data
    std::string buffer = socketServer->recvMessage();
    if (buffer != "") {
        socketServer->sendMessage(buffer);
        parseLandmarkData(buffer);
    }

    // Update frame timing
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    this->frame_timing_ = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->last_frame_time_);
    this->last_frame_time_ = now;

    // Collect pose data of other hardware
    int HMDidx = 0;
    vr::TrackedDevicePose_t* rawPoseData = new vr::TrackedDevicePose_t[vr::k_unMaxTrackedDeviceCount];
    vr::VRServerDriverHost()->GetRawTrackedDevicePoses(0, rawPoseData, vr::k_unMaxTrackedDeviceCount);
    vr::CVRPropertyHelpers* props = vr::VRProperties();
    for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
        vr::PropertyContainerHandle_t container = GetProperties()->TrackedDeviceToPropertyContainer(i);
        vr::ETrackedPropertyError err;
        int32_t result = props->GetInt32Property(container, vr::ETrackedDeviceProperty::Prop_DeviceClass_Int32, &err);
        if (result == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD)
        {
            HMDidx = i;
            break;
        }
    }
    // Get properties of Headset
    vr::PropertyContainerHandle_t HMDcontainer = GetProperties()->TrackedDeviceToPropertyContainer(HMDidx);
    vr::HmdMatrix34_t HMDpose = rawPoseData[0].mDeviceToAbsoluteTracking;

    // Set the trackers equal to the hip (supposedly)
    double centerHipX, centerHipY, centerHipZ;
    centerHipX = HMDpose.m[0][3] + poseData[0][0];
    centerHipY = HMDpose.m[1][3] + poseData[0][1];
    centerHipZ = HMDpose.m[2][3] + poseData[0][2];
    for (auto& device : this->devices_) {
        device->Update();
        if (device->GetDeviceType() == DeviceType::TRACKER) {
            device->setPose(centerHipX, centerHipY, centerHipZ);
        }
        else {
            socketServer->sendMessage("Device " + device->GetSerial() + " not a tracker. Skipping...");
        }
    }
    delete[] rawPoseData;

}

bool ExampleDriver::VRDriver::ShouldBlockStandbyMode()
{
    return false;
}

void ExampleDriver::VRDriver::EnterStandby()
{
}

void ExampleDriver::VRDriver::LeaveStandby()
{
}

std::vector<std::shared_ptr<ExampleDriver::IVRDevice>> ExampleDriver::VRDriver::GetDevices()
{
    return this->devices_;
}

std::vector<vr::VREvent_t> ExampleDriver::VRDriver::GetOpenVREvents()
{
    return this->openvr_events_;
}

std::chrono::milliseconds ExampleDriver::VRDriver::GetLastFrameTime()
{
    return this->frame_timing_;
}

bool ExampleDriver::VRDriver::AddDevice(std::shared_ptr<IVRDevice> device)
{
    vr::ETrackedDeviceClass openvr_device_class;
    // Remember to update this switch when new device types are added
    switch (device->GetDeviceType()) {
        case DeviceType::CONTROLLER:
            openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_Controller;
            break;
        case DeviceType::HMD:
            openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_HMD;
            break;
        case DeviceType::TRACKER:
            openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker;
            break;
        case DeviceType::TRACKING_REFERENCE:
            openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference;
            break;
        default:
            return false;
    }
    bool result = vr::VRServerDriverHost()->TrackedDeviceAdded(device->GetSerial().c_str(), openvr_device_class, device.get());
    if(result)
        this->devices_.push_back(device);
    return result;
}

ExampleDriver::SettingsValue ExampleDriver::VRDriver::GetSettingsValue(std::string key)
{
    vr::EVRSettingsError err = vr::EVRSettingsError::VRSettingsError_None;
    int int_value = vr::VRSettings()->GetInt32(settings_key_.c_str(), key.c_str(), &err);
    if (err == vr::EVRSettingsError::VRSettingsError_None) {
        return int_value;
    }
    err = vr::EVRSettingsError::VRSettingsError_None;
    float float_value = vr::VRSettings()->GetFloat(settings_key_.c_str(), key.c_str(), &err);
    if (err == vr::EVRSettingsError::VRSettingsError_None) {
        return float_value;
    }
    err = vr::EVRSettingsError::VRSettingsError_None;
    bool bool_value = vr::VRSettings()->GetBool(settings_key_.c_str(), key.c_str(), &err);
    if (err == vr::EVRSettingsError::VRSettingsError_None) {
        return bool_value;
    }
    std::string str_value;
    str_value.reserve(1024);
    vr::VRSettings()->GetString(settings_key_.c_str(), key.c_str(), str_value.data(), 1024, &err);
    if (err == vr::EVRSettingsError::VRSettingsError_None) {
        return str_value;
    }
    err = vr::EVRSettingsError::VRSettingsError_None;

    return SettingsValue();
}

void ExampleDriver::VRDriver::Log(std::string message)
{
    std::string message_endl = message + "\n";
    vr::VRDriverLog()->Log(message_endl.c_str());
}

vr::IVRDriverInput* ExampleDriver::VRDriver::GetInput()
{
    return vr::VRDriverInput();
}

vr::CVRPropertyHelpers* ExampleDriver::VRDriver::GetProperties()
{
    return vr::VRProperties();
}

vr::IVRServerDriverHost* ExampleDriver::VRDriver::GetDriverHost()
{
    return vr::VRServerDriverHost();
}

// function that takes in a std::string of data and returns a 2 dimensional list of landmark coordinates
// important shit for wherever I run this function
// float landmark[33][3];
// parse_landmark(buffer, landmark);
int ExampleDriver::VRDriver::parseLandmarkData(std::string buffer)
{
    // create stream for easy iteration over words
    std::istringstream iss(buffer);
    // iterate over string, convert each element to a float and stick into landmark
    for (int i = 0; i < 33; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // read from string if it's not empty, otherwise end
            if (iss)
            {
                // store next stream value in a temp before updating landmark
                std::string temp;
                iss >> temp;
                poseData[i][j] = std::stod(temp);
            }
            else
            {
                return 0;
            }
        }
    }


}