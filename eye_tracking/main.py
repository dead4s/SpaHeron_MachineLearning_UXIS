import cv2
import time

from eye_tracking import EyeTracking

eye_tracking = EyeTracking()
webcam = cv2.VideoCapture(0)

while True:
    _, frame = webcam.read()

    if frame is None:
        break

    eye_tracking.refresh(frame)
    frame = eye_tracking.annotated_frame()
    text = ""
    attention_text = ""

    if eye_tracking.is_blinking():
        text = "Blinking"
    elif eye_tracking.is_right():
        # print('right')
        eye_tracking.is_attention -= 1
        text = "Looking right"
    elif eye_tracking.is_left():
        # print('left')
        eye_tracking.is_attention -= 1
        text = "Looking left"
    elif eye_tracking.is_center():
        # print('center')
        eye_tracking.is_attention += 1
        text = "Looking center"

    # attention example
    if eye_tracking.is_attention > 100:
        eye_tracking.is_attention = 100
    elif eye_tracking.is_attention < 0:
        eye_tracking.is_attention = 0

    if eye_tracking.is_attention < 10:
        attention_text = "Cheer up"
    else:
        attention_text = "Good!"

    if eye_tracking.is_focus():
        print('focus!')
    else:
        print('hey!')
    
    cv2.putText(frame, text, (90, 60), cv2.FONT_HERSHEY_DUPLEX, 1.6, (147, 58, 31), 2)

    left_pupil = eye_tracking.pupil_left_coords()
    right_pupil = eye_tracking.pupil_right_coords()
    attention = eye_tracking.is_attention
    method = eye_tracking.get_method()

    cv2.putText(frame, "Left pupil:  " + str(left_pupil), (90, 130), cv2.FONT_HERSHEY_DUPLEX, 0.9, (147, 58, 31), 1)
    cv2.putText(frame, "Right pupil: " + str(right_pupil), (90, 165), cv2.FONT_HERSHEY_DUPLEX, 0.9, (147, 58, 31), 1)
    cv2.putText(frame, "Attention: " + str(attention), (90, 200), cv2.FONT_HERSHEY_DUPLEX, 0.9, (147, 58, 31), 1)
    cv2.putText(frame, method, (90, 235), cv2.FONT_HERSHEY_DUPLEX, 0.9, (147, 58, 31), 1)
    cv2.putText(frame, "Focus?: " + attention_text, (90, 270), cv2.FONT_HERSHEY_DUPLEX, 0.9, (147, 58, 31), 1)

    cv2.imshow("Demo", frame)

    # esc key
    if cv2.waitKey(10) == 27:
        break

webcam.release()
cv2.destroyAllWindows()