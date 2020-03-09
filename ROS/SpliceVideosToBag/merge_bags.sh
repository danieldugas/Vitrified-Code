### This project spliced videos into a pre-existing rosbag, in order to show various camera views in rviz
### alongside the sensor data interactively.

# Original videos are already scaled down, with 20-30fps
cd ~/Code/pyniel/pyniel/ros_tools/
./video_to_rosbag ~/Videos/pepper_demo/camera_follow.mp4 /media/storage/camera_follow.bag
./video_to_rosbag ~/Videos/pepper_demo/camera_fixed_north.mp4 /media/storage/camera_fixed_north.bag
./video_to_rosbag ~/Videos/pepper_demo/camera_fixed_koze.mp4 /media/storage/camera_fixed_koze.bag

# Modified add_header_time_offset to remap topics
cd /media/storage
rosrun bag_tools add_header_time_offset.py -o  1536923857.229687 -i /media/storage/camera_follow.bag -t camera/image_raw -m /camera_follow/image_raw
rosrun bag_tools add_header_time_offset.py -o  1536923835.229687 -i /media/storage/camera_fixed_north.bag -t camera/image_raw -m /camera_fixed_north/image_raw
rosrun bag_tools add_header_time_offset.py -o  1536923803.229687 -i /media/storage/camera_fixed_koze.bag -t camera/image_raw -m /camera_fixed_koze/image_raw
# offsets calculated from video offsets and main bag offset (see bagsync.txt)

rosrun bag_tools replace_msg_time_with_hdr.py -o camera_follow-deepsync.bag -i camera_follow-synced.bag
rosrun bag_tools replace_msg_time_with_hdr.py -o camera_fixed_north-deepsync.bag -i camera_fixed_north-synced.bag
rosrun bag_tools replace_msg_time_with_hdr.py -o camera_fixed_koze-deepsync.bag -i camera_fixed_koze-synced.bag
rm *-synced.bag

rosbag reindex camera_follow-deepsync.bag
rm *.orig.bag
rosbag reindex camera_fixed_north-deepsync.bag
rm *.orig.bag
rosbag reindex camera_fixed_koze-deepsync.bag
rm *.orig.bag


# I expected to have to use -i (reindex), but it does not work. Don't use -i instead.
cd ~/Code/pepper_ws/src/bagedit/nodes
./bagmerge.py -o /media/storage/merged.bag ~/rosbags/demo2.bag /media/storage/camera_follow-deepsync.bag
./bagmerge.py -o /media/storage/merged2.bag /media/storage/merged.bag /media/storage/camera_fixed_north-deepsync.bag
rm /media/storage/merged.bag # save some space
./bagmerge.py -o /media/storage/merged3.bag /media/storage/merged2.bag /media/storage/camera_fixed_koze-deepsync.bag

rm /media/storage/merged2.bag
rm /media/storage/*-deepsync.bag
