// Copyright 2017 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// An example of sending OpenCV webcam frames into a MediaPipe graph.

#include <stdlib.h> // system func()
#include <unistd.h> // sleep func()
#include <string.h> // strcpy func()
#include <fcntl.h>
#include <sys/stat.h>

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
//#include "mediapipe/framework/port/commandlineflags.h"
#include "mediapipe/framework/port/file_helpers.h"
#include "mediapipe/framework/port/opencv_highgui_inc.h"
#include "mediapipe/framework/port/opencv_imgproc_inc.h"
#include "mediapipe/framework/port/opencv_video_inc.h"
#include "mediapipe/framework/port/parse_text_proto.h"
#include "mediapipe/framework/port/status.h"

//Take stream from /mediapipe/graphs/iris_tracking/iris_detection_cpu.pbtxt
// RendererSubgraph - LANDMARKS:iris_landmarks
#include "mediapipe/calculators/util/landmarks_to_render_data_calculator.pb.h"
#include "mediapipe/framework/formats/landmark.pb.h"

#define PIPENAME "/home/pipe/pipe_name"

typedef struct data {
    int look;
    int student_id;
} DATA;

DATA data;

// input and output streams to be used/retrieved by calculators
constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";
constexpr char kLandmarksTest[] = "face_landmarks";
constexpr char kLandmarksStream[] = "iris_landmarks";
constexpr char kWindowName[] = "MediaPipe";

float left_left, left_right, right_left, right_right;

// cli inputs
DEFINE_string(
    calculator_graph_config_file, "",
    "Name of file containing text format CalculatorGraphConfig proto.");
DEFINE_string(input_video_path, "",
              "Full path of video to load. "
              "If not provided, attempt to use a webcam.");

::mediapipe::Status RunMPPGraph(std::string input_path) {
  std::string calculator_graph_config_contents;
  MP_RETURN_IF_ERROR(mediapipe::file::GetContents(
      FLAGS_calculator_graph_config_file, &calculator_graph_config_contents));
  LOG(INFO) << "Get calculator graph config contents: "
            << calculator_graph_config_contents;
  mediapipe::CalculatorGraphConfig config =
      mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
          calculator_graph_config_contents);

  LOG(INFO) << "Initialize the calculator graph.";
  mediapipe::CalculatorGraph graph;
  MP_RETURN_IF_ERROR(graph.Initialize(config));

  LOG(INFO) << "Initialize the camera or load the video.";
  cv::VideoCapture capture;
  const bool load_video = !input_path.empty();
  if (load_video) {
    capture.open(input_path);
  } else {
    capture.open(0);
  }
  RET_CHECK(capture.isOpened());

  cv::VideoWriter writer;
  const bool save_video = !input_path.empty();

  // pollers to retrieve streams from graph
  // output stream (i.e. rendered landmark frame)
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                   graph.AddOutputStreamPoller(kOutputStream));

  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller_face_landmark,
            graph.AddOutputStreamPoller(kLandmarksTest));
  // iris landmarks stream
  ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller_iris_landmark,
            graph.AddOutputStreamPoller(kLandmarksStream));

  LOG(INFO) << "Start running the calculator graph.";
  MP_RETURN_IF_ERROR(graph.StartRun({}));

  LOG(INFO) << "Start grabbing and processing frames.";
  size_t frame_timestamp = 0;
  bool grab_frames = true;
  while (grab_frames) {
    // Capture opencv camera or video frame.
    cv::Mat camera_frame_raw;
    capture >> camera_frame_raw;
    if (camera_frame_raw.empty()) break;  // End of video.
    cv::Mat camera_frame;
    cv::cvtColor(camera_frame_raw, camera_frame, cv::COLOR_BGR2RGB);
    if (!load_video) {
      cv::flip(camera_frame, camera_frame, /*flipcode=HORIZONTAL*/ 1);
    }

    // Wrap Mat into an ImageFrame.
    auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
        mediapipe::ImageFormat::SRGB, camera_frame.cols, camera_frame.rows,
        mediapipe::ImageFrame::kDefaultAlignmentBoundary);
    cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
    camera_frame.copyTo(input_frame_mat);

    // Send image packet into the graph.
    MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
        kInputStream, mediapipe::Adopt(input_frame.release())
                          .At(mediapipe::Timestamp(frame_timestamp++))));

    // Get the graph result packet, or stop if that fails.
    ::mediapipe::Packet packet;
    ::mediapipe::Packet iris_landmark_packet, face_landmark_packet;

    // Polling the poller to get landmark packet
    if (!poller.Next(&packet)) break;
    if (!poller_face_landmark.Next(&face_landmark_packet)) break;
    if (!poller_iris_landmark.Next(&iris_landmark_packet)) break;

    // Use packet.Get to recover values from packet
    auto& output_frame = packet.Get<mediapipe::ImageFrame>();
    auto& output_face = face_landmark_packet.Get<mediapipe::NormalizedLandmarkList>();
    auto& output_iris = iris_landmark_packet.Get<mediapipe::NormalizedLandmarkList>();

    mediapipe::NormalizedLandmark l, r;
    float left_eye, right_eye, left_pupil, right_pupil;

    l = output_iris.landmark(0);
    r = output_iris.landmark(5);
    left_pupil = l.x();
    right_pupil = r.x();

    l = output_face.landmark(33);
    r = output_face.landmark(133);
    left_left = left_pupil - l.x();
    left_right = r.x() - left_pupil;

    l = output_face.landmark(362);
    r = output_face.landmark(263);
    right_left = right_pupil - l.x();
    right_right = r.x() - right_pupil;

    // Convert back to opencv for display or saving.
    cv::Mat output_frame_mat = mediapipe::formats::MatView(&output_frame);
    cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);
  }

  LOG(INFO) << "Shutting down.";
  MP_RETURN_IF_ERROR(graph.CloseInputStream(kInputStream));
  return graph.WaitUntilDone();
}

std::string Img2Vid(int i) {
	std::string input_path = FLAGS_input_video_path + "/" + std::to_string(i) + ".jpg";
  std::string output_path = FLAGS_input_video_path + "/" + std::to_string(i) + ".mp4";
  std::string s = "ffmpeg -loop 1 -f image2 -t 0.07 -i " + input_path + " " + output_path;
	std::cout << output_path << std::endl;

  char ch[100];
  std::strcpy(ch, s.c_str());
  std::system(ch);

  return output_path;
}

int SendData() {
  int fd;

  if((fd = open(PIPENAME, O_WRONLY)) < 0) {
    std::cout << "Fail to open pipe\n";
    return -1;
  }

  write(fd, &data, sizeof(DATA));
  return 0;
}

void DataSave(int i) {
  data.student_id = 1000;

  float left_judge = left_left - left_right;
  float right_judge = right_left - right_right;

  // center
  if(abs(left_judge) <= 0.002 || abs(right_judge) <= 0.002)
    data.look = 0;
  // right
  else if(left_judge < 0 && right_judge < 0)
    data.look = 1;
  // left
  else if(left_judge > 0 && right_judge > 0)
    data.look = 2;
  else
    data.look = -1;

  SendData();
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, true);

	int i = 0;
	while(i < 30) {
  	::mediapipe::Status run_status = RunMPPGraph(Img2Vid(i));
	  if (!run_status.ok()) {
  	  LOG(ERROR) << "Failed to run the graph: " << run_status.message();
	  } else {
  	  LOG(INFO) << "Success!";
			DataSave(i);
	  }
		i++;
		sleep(1);
	}

  return 0;
}

