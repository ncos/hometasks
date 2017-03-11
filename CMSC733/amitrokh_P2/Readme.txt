Go to Code and run swap.py -h to see the list of all options. This stuff requires OpenCV 3.0 (as our TA has kindly mentioned), which is easy to install and which comes with ROS on Ubuntu 16.

Important options are:

  -s SOURCE_IMG, --source=SOURCE_IMG
                        specify path to source picture
  -t TARGET_IMG, --target=TARGET_IMG
                        specify path to target image
  -v TARGET_VIDEO_PATH, --target_video=TARGET_VIDEO_PATH
                        specify path to target video

if -s and -t specified, it will replace face from -t with one from -s
if -s and -v specified, it will replace face from -v with one from -s
if -s is not specified, swap.py will try to swap faces on -t or -v

  --method=METHOD       specify swap method: 'TRI' for triangulation based one
                        or 'TPS' for Thin Plate Spline method
you can choose which method to use, although 'TPS' is faster and supports multithreading

use --debug-level=1 to show landmarks on the image

Examples:
./swap.py -s ../TestSet/Rambo.jpg -v ../TestSet/Test1.mp4 # Replace face in Test1.mp4 with face on Rambo.jpg
./swap.py -v ../TestSet/Test2.mp4 # Swap faces in this video
