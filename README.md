# SpaHeron_MachineLearning_UXIS
![80832108](https://user-images.githubusercontent.com/38307839/165902850-34d2bdf0-3e59-4b8b-9bd9-a7928d44c035.jpeg)

Lecture score calculator with iris location using madiapipe.

## eye_tracking
- Based on https://github.com/antoinelame/GazeTracking
- Print pupil's position, using HOG + DNN(maybe?) face detection.
- Detect face with HOG. Try detecting face with DNN, if face is not detected.
- While HOG prints pupil properly, DNN prints only CENTER of eye.
- Webcam version.

## mediapipe
- Based on https://github.com/google/mediapipe
- Edit iris_trakcing part.
- Calculate both eye's iris location.
- The result of output is as followed.
<br><img src="https://user-images.githubusercontent.com/38307839/123956679-eb19b200-d9e5-11eb-8d54-0d26024986d3.png" alt="seung" width="250"><br>*OH MY GIRL - SEUNG*

## State Calculation
- Communicate with mediapipe program by 'Named pipe' to share iris's location(see mediapipe/pipe).
- State value: 0 - ATTEND, 1 - ABSCENT, 2 - CONCENTRATE, 3 - DISTRACT
- Structure
<br><img src="https://user-images.githubusercontent.com/38307839/123957563-e99cb980-d9e6-11eb-8b41-558422c8df70.png" alt="seung" width="500">

## ML
- Score prediction using multiple linear regression.
- Input: an array with 4 states counted individually / output: lecture score.
