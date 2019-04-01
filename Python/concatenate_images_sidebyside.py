from PIL import Image
import os
import sys

left_images_folder = "./frames"
right_images_folder = "./frames2"

left_images = sorted(os.listdir(left_images_folder))
right_images = sorted(os.listdir(right_images_folder))

if len(left_images) != len(right_images):
    raise IndexError

for left, right in zip(left_images, right_images):
    left_im = Image.open(os.path.join(left_images_folder, left))
    right_im = Image.open(os.path.join(right_images_folder, right))

    left_im.thumbnail((1000, 320), Image.ANTIALIAS)
    right_im.thumbnail((1000, 320), Image.ANTIALIAS)

    total_width = left_im.size[0] + right_im.size[0]
    combined = Image.new('RGB', (total_width, 320))
    combined.paste(left_im, (0,0))
    combined.paste(right_im, (left_im.size[0], 0))

    combined_path = os.path.join("combined", left)
    combined.save(combined_path)
    print("saving {} / {}".format(combined_path, left_images[-1]))
