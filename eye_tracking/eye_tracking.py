import os
import cv2
import dlib
import numpy as np
from eye import Eye
from calibration import Calibration


class EyeTracking(object):
    """
    This class tracks the user's gaze.
    It provides useful information like the position of the eyes
    and pupils and allows to know if the eyes are open or closed
    """

    def __init__(self):
        self.frame = None        
        self.calibration = Calibration()

        self.eye_left = None
        self.eye_right = None
        self.ex_eye_left = None
        self.ex_eye_right = None

        self.is_attention = 100
        self.method = ""

        # _face_detector is used to detect faces
        self._face_detector = dlib.get_frontal_face_detector()

        # _predictor is used to get facial landmarks of a given face
        cwd = os.path.abspath(os.path.dirname(__file__))
        model_path = os.path.abspath(os.path.join(cwd, "./models/shape_predictor_68_face_landmarks.dat"))
        self._predictor = dlib.shape_predictor(model_path)

        # dnn based landmark
        self.net = cv2.dnn.readNetFromCaffe(
            './models/deploy.prototxt.txt',
            './models/res10_300x300_ssd_iter_140000.caffemodel'
        )
        self.landmark_predictor = dlib.shape_predictor('./models/shape_predictor_68_face_landmarks.dat')

    @property
    def pupils_located(self):
        """Check that the pupils have been located"""
        try:
            int(self.eye_left.pupil.x)
            int(self.eye_left.pupil.y)
            int(self.eye_right.pupil.x)
            int(self.eye_right.pupil.y)
            return True
        except Exception:
            return False

    def adjust_gamma(self, image, gamma):
        invGamma = 1.0 / gamma
        table = np.array([((i / 255.0) ** invGamma) * 255
            for i in np.arange(0, 256)]).astype("uint8")
    
        return cv2.LUT(image, table)

    def preprocess(self, img):
        gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        if gray_img.mean() < 130:
                img = self.adjust_gamma(img, 1.0)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        return img


    def _analyze(self):
        """Detects the face and initialize Eye objects"""
        # 필터링의 중요성 preprocess를 하고 안 하고에 따라 결과가 달라진다.
        # dnn에 감마 값을 잘 조절하면 좋지 않을까.
        frame = cv2.cvtColor(self.frame, cv2.COLOR_BGR2GRAY)
        # frame = self.preprocess(self.frame)
        faces = self._face_detector(frame)

        try:
            if faces:
                self.method = "hog"

                landmark = np.empty([68, 2], dtype = int)
                landmarks = self._predictor(frame, faces[0])
                    
                for i in range(68):
                    landmark[i][0] = landmarks.part(i).x
                    landmark[i][1] = landmarks.part(i).y
                # print(landmark)
    
                self.eye_left = Eye(frame, landmark, 0, self.calibration)
                self.eye_right = Eye(frame, landmark, 1, self.calibration)

            else:
                self.method = "dnn"

                (h, w) = self.frame.shape[:2]

                blob = cv2.dnn.blobFromImage(
                    cv2.resize(self.frame, (300, 300)),
                        1.0,(300, 300), (104.0, 177.0, 123.0
                    )
                )
                self.net.setInput(blob)
                detections = self.net.forward()

                ## bounding box
                list_bboxes = []
                list_confidence = []
                list_dlib_rect = []

                for i in range(0, detections.shape[2]):
                    confidence = detections[0, 0, i, 2]

                    if confidence < 0.6:
                        continue

                    box = detections[0, 0, i, 3:7] * np.array([w, h, w, h])
                    (l, t, r, b) = box.astype('int')

                    original_vertical_length = b - t
                    t = int(t + original_vertical_length * 0.15)
                    b = int(b - (original_vertical_length) * 0.05)

                    margin = ((b-t) - (r-l))//2
                    l = l - margin if (b-t-r+l)%2 == 0 else l - margin - 1
                    r = r + margin
                    list_bboxes.append([l, t, r, b])
                    list_confidence.append(confidence)
                    rect_bb = dlib.rectangle(left=l, top=t, right=r, bottom=b)
                    list_dlib_rect.append(rect_bb)

                # landmark
                list_landmarks = []

                for rect in list_dlib_rect:
                    points = self.landmark_predictor(self.frame, rect)
                    list_points = list(map(lambda p: (p.x, p.y), points.parts()))
                    list_landmarks.append(list_points)
                    # print(list_landmarks)

                    list_landmarks = list_landmarks[0]

                    self.eye_left = Eye(self.frame, list_landmarks, 0, self.calibration)
                    self.eye_right = Eye(self.frame, list_landmarks, 1, self.calibration)

        except IndexError:
            self.eye_left = None
            self.eye_right = None
        

    def refresh(self, frame):
        """Refreshes the frame and analyzes it.

        Arguments:
            frame (numpy.ndarray): The frame to analyze
        """
        self.frame = frame
        self._analyze()

    def pupil_left_coords(self):
        """Returns the coordinates of the left pupil"""
        if self.pupils_located:
            x = self.eye_left.origin[0] + self.eye_left.pupil.x
            y = self.eye_left.origin[1] + self.eye_left.pupil.y
            return (x, y)

    def pupil_right_coords(self):
        """Returns the coordinates of the right pupil"""
        if self.pupils_located:
            x = self.eye_right.origin[0] + self.eye_right.pupil.x
            y = self.eye_right.origin[1] + self.eye_right.pupil.y
            return (x, y)

    def get_attention(self):
        return self.is_attention

    def get_method(self):
        return self.method

    def horizontal_ratio(self):
        """Returns a number between 0.0 and 1.0 that indicates the
        horizontal direction of the gaze. The extreme right is 0.0,
        the center is 0.5 and the extreme left is 1.0
        """
        if self.pupils_located:
            pupil_left = self.eye_left.pupil.x / (self.eye_left.center[0] * 2 - 10)
            pupil_right = self.eye_right.pupil.x / (self.eye_right.center[0] * 2 - 10)
            return (pupil_left + pupil_right) / 2

    def vertical_ratio(self):
        """Returns a number between 0.0 and 1.0 that indicates the
        vertical direction of the gaze. The extreme top is 0.0,
        the center is 0.5 and the extreme bottom is 1.0
        """
        if self.pupils_located:
            pupil_left = self.eye_left.pupil.y / (self.eye_left.center[1] * 2 - 10)
            pupil_right = self.eye_right.pupil.y / (self.eye_right.center[1] * 2 - 10)
            return (pupil_left + pupil_right) / 2

    def is_right(self):
        """Returns true if the user is looking to the right"""
        if self.pupils_located:
            return self.horizontal_ratio() <= 0.35

    def is_left(self):
        """Returns true if the user is looking to the left"""
        if self.pupils_located:
            return self.horizontal_ratio() >= 0.65

    def is_center(self):
        """Returns true if the user is looking to the center"""
        if self.pupils_located:
            return self.is_right() is not True and self.is_left() is not True

    def is_blinking(self):
        """Returns true if the user closes his eyes"""
        if self.pupils_located:
            blinking_ratio = (self.eye_left.blinking + self.eye_right.blinking) / 2
            return blinking_ratio > 3.8

    def is_focus(self):
        if self.ex_eye_left is None:
            self.eye_position_update()

            return 0

        if self.pupils_located:
            focus = (
                ((self.eye_left.pupil.x + self.eye_right.pupil.x) / 2) -
                ((self.ex_eye_left.pupil.x + self.ex_eye_right.pupil.x) / 2)
            )

            self.eye_position_update()

            if abs(focus) < 5:
                return 1
            else:
                return 0

    def eye_position_update(self):
        self.ex_eye_left = self.eye_left
        self.ex_eye_right = self.eye_right

    def annotated_frame(self):
        """Returns the main frame with pupils highlighted"""
        frame = self.frame.copy()

        if self.pupils_located:
            color = (0, 255, 0)
            x_left, y_left = self.pupil_left_coords()
            x_right, y_right = self.pupil_right_coords()

            cv2.line(frame, (x_left - 5, y_left), (x_left + 5, y_left), color)
            cv2.line(frame, (x_left, y_left - 5), (x_left, y_left + 5), color)
            cv2.line(frame, (x_right - 5, y_right), (x_right + 5, y_right), color)
            cv2.line(frame, (x_right, y_right - 5), (x_right, y_right + 5), color)

        return frame
