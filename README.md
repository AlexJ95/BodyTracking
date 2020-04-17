# Real-time body reconstruction and recognition in virtual reality using Vive Trackers and Controllers

This project implements an accurate full-body recognition approach for VR-based applications using HTC Vive Trackers and Controllers. Motion recognition is based on Hidden Markov Model. Currently, a Yoga Warrior I pose can be recognized with an accuracy of 88%. However, more models can be trained. Furthermore, we implemented an Inverse Kinematics solver to animate the motions of the avatar as smoothly, rapidly and as accurately as possible. Using a HTC Vive headset, two Vive Controllers and five Vive Trackers, attached to the hands and feet, it is possible to create immersive VR experiences, where the user is able to perceive the avatar as her/his own body. The user can see her/his avatar from the first-person perspective and in a virtual mirror.

## Requirements:
- node.js
- VisualStudio 2019 or Xcode

## Clone the project:
git clone --recursive git@github.com:CatCuddler/BodyTracking.git <br />
cd BodyTracking/BodyModel/Kore <br />
git checkout master <br />
git submodule update --init --recursive <br />
cd .. <br />

### Compile
node Kore/make --vr steamvr

#### Only for Mac Users:
Currently Metal does not work. Use Opengl. <br />
node Kore/make -g opengl

#### Only for Windows Users:
If you dont use Visual Studio 2019, you can specify the used version: <br />
node Kore/make -v vs20xx

### Open the Project:
Open VisualStudio or Xcode project in BodyModel/build. <br />
Change to "Develop x86" (Release doent work) mode in Visual Studio. <br />
Change working directory in Xcode: Edit Scheme -> Use custom working directory -> choose deployment directory.

### Avatar Calibration
1. Strap one Vive Tracker on your left foot and another one on your right foot (above ankles)
2. Strap the third Vive tracker on your waist
3. Strap the fourth and fifth tracker on the left and right forearm.
4. Hold the Vive Controller in your hands ;)
5. Start the project. You will see an avatar, standing in a T-Pose.
6. Press the "grip button" to set the size of the avatar (you must look straight ahead).
7. Go to where the avatar stands and put your feet and hands in the same position.
8. Make sure that the green arrows for the controllers are pointing straight ahead.
9. Press the "menu button" to calibrate the avatar.

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/x4SS8_-XY38/0.jpg)](https://youtu.be/x4SS8_-XY38)

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/F6yFdpnhhoo/0.jpg)](https://youtu.be/F6yFdpnhhoo)

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/q-yKLtrTodA/0.jpg)](https://youtu.be/q-yKLtrTodA)
