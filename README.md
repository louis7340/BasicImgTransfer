#Basic Image Processing
1.Read `.pcd` file and transfer to a bmp file

2.Do flip vertical by pressing 'r' or 'R' 

3.Draw a rectangle and turn rgb info to grayscale value within rectangle

##Install opencv-2.4.9
####1. UPDATE AND UPGRADE UBUNTU
Open your terminal and execute:
```
sudo apt-get update
sudo apt-get upgrade
```
####2. INSTALL THE DEPENDENCIES
Now execute:
```
sudo apt-get install build-essential libgtk2.0-dev libjpeg-dev libtiff4-dev libjasper-dev libopenexr-dev cmake python-dev python-numpy python-tk libtbb-dev libeigen2-dev yasm libfaac-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev libx264-dev libqt4-dev libqt4-opengl-dev sphinx-common texlive-latex-extra libv4l-dev libdc1394-22-dev libavcodec-dev libavformat-dev libswscale-dev
```
####3. DOWNLOAD AND DECOMPRESS OPENCV
Enter the [OpenCV official website](http://opencv.org/) and download the latest version for Ubuntu Linux. Then decompress the downloaded file

####4. COMPILE OPENCV
Now, in your terminal, make sure you are within the OpenCV directory and run the following commands:
```
mkdir build
cd build 
cmake -D WITH_TBB=ON -D BUILD_NEW_PYTHON_SUPPORT=ON -D WITH_V4L=ON -D INSTALL_C_EXAMPLES=ON -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_EXAMPLES=ON -D WITH_QT=ON -D WITH_OPENGL=ON .. 
make
sudo make install 
```
####5. CONFIGURE OPENCV
In your terminal, execute:

`sudo gedit /etc/ld.so.conf.d/opencv.conf`

Add the following line and save it:

`/usr/local/lib`

Now, in your terminal, execute:

`sudo ldconfig`

Again, execute:

`sudo gedit /etc/bash.bashrc`

Add the following two lines at the end of the file and save it:
```
PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig
export PKG_CONFIG_PATH
```
Finally, reboot your computer.
####6. TRY AN EXAMPLE
To test an example, from your terminal enters in the OpenCV folder. Once you're inside it enters in samples/c.
Now, execute:
```
chmod +x build_all.sh
./build_all.sh
./facedetect lena.jpg 
```

![lena](https://github.com/louis7340/BasicImgTransfer/blob/master/images/lenaopencv.png)

##Compile

`gcc ImgTransfer.c -o ImgTransfer !`pkg-config --cflags --libs opencv!` `
	


