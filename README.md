# RTSP Server
## Pre-requisites
**All**
```
	sudo apt-get install -y  \
		libgstreamer1.0-dev \
		libgstreamer-plugins-base1.0-dev \
		libgstreamer-plugins-bad1.0-dev \
		gstreamer1.0-plugins-ugly \
		gstreamer1.0-tools \
		gstreamer1.0-gl \
		gstreamer1.0-gtk3 \
		gstreamer1.0-rtsp
```
**Pi**
```
sudo apt-get install -y gstreamer1.0-libcamera
```
**Ubuntu 22.04**, see https://github.com/antimof/UxPlay/issues/121
```
sudo apt remove gstreamer1.0-vaapi
```

## Build and run
Build
```
make
```
Run
```
./build/rtsp-server
```
Play the stream
```
gst-launch-1.0 playbin uri=rtsp://192.168.68.77:8554/test
```
or
```
gst-launch-1.0 rtspsrc location=rtsp://192.168.68.77:8554/test latency=0 ! rtph264depay ! avdec_h264 ! videoconvert ! autovideosink
```
