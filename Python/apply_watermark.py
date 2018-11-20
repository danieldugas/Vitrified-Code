import os
import numpy as np
from PIL import Image, ImageDraw, ImageFont, ImageFilter


# Parameters
# font = ImageFont.truetype('/usr/share/fonts/truetype/lane/LANENAR_.ttf', 20)
font = ImageFont.truetype('/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-L.ttf', 14)
kNewDir = "../EarthviewWallpapersWatermarked"
kShadowRadius = 2 # points
kTextPad = 30
TEXT_PLACEMENT = 2
# 1 =    top_left, 0 =    top_middle, 7 =    top_right,
# 2 = middle_left,                  , 6 = middle_right,
# 3 = bottom_left, 4 = bottom_middle, 5 = bottom_right,
CHAMELEON_TEXTCOLOR = False
BLUR = False
SHADOW = False
ROTATE = True

SCREEN_ASPECT_RATIO = 16. / 10. # 1920 * 1200 (16:10) | 1920 * 1080  (16:9)

files = sorted(os.listdir("."))
texts = [os.path.splitext(path)[0].replace("_", " ") for path in files]
texts = [''.join(c for c in text if c.isdigit()) for text in texts]
try:
    os.makedirs("../EarthviewWallpapersWatermarked")
except:
    print()

for i, (path, text) in enumerate(zip(files, texts)):
    print("{} / {} : {}".format(i, len(files), text))
    try:
        photo = Image.open(path).convert('RGBA')
    except:
        print("Could not open, skipped.")
        continue
    size = photo.size
    if ROTATE:
        size = (photo.size[1], photo.size[0]-1)
    txt = Image.new('RGBA', size, (255,255,255,0))
    blur = Image.new('RGBA', size, (255,255,255,0))
    d = ImageDraw.Draw(txt)
    b = d
    if BLUR:
        b = ImageDraw.Draw(blur)
    w, h = txt.size
    t_w, t_h = font.getsize(text)
    # Text Placement
    if ROTATE:
        text_placement = ( TEXT_PLACEMENT - 2 ) % 8
    if text_placement == 4: # "bottom_middle"
        pos = (int(w/2 - t_w/2),int(h - t_h - kTextPad))
    elif text_placement == 0: # "top_middle"
        pos = (int(w/2 - t_w/2),int(kTextPad))
    elif text_placement == 2: # "middle_left"
        pos = (int(kTextPad),int(h/2 - t_h/2))
    else:
        raise ValueError("unexpected value for TEXT_PLACEMENT")
    # Extra positions to thicken shadow
    posr = (pos[0]+1, pos[1])
    posl = (pos[0]-1, pos[1])
    posu = (pos[0], pos[1]+1)
    posd = (pos[0], pos[1]-1)
    # Text color
    if CHAMELEON_TEXTCOLOR:
        textcolor = np.average([photo.getpixel(pos), photo.getpixel(posr), photo.getpixel(posl)], axis=0)
        textcolor = textcolor
        textcolor = tuple(textcolor.astype(int))
    else:
        textcolor = (245, 245, 245, 240)
    if SHADOW:
        shadowcolor = (255, 255, 255, 128)
    # Draw text and shadow text
    if SHADOW:
        b.text(pos , text, fill=shadowcolor, font=font)
        b.text(posr, text, fill=shadowcolor, font=font)
        b.text(posl, text, fill=shadowcolor, font=font)
        b.text(posu, text, fill=shadowcolor, font=font)
        b.text(posd, text, fill=shadowcolor, font=font)
    d.text(pos, text, fill=textcolor, font=font)
    # Blur shadow text
    if BLUR:
        blur = blur.filter(ImageFilter.GaussianBlur(kShadowRadius))
    # Rotate
    if ROTATE:
        blur = blur.rotate(90, expand=1)
        txt  = txt.rotate(90, expand=1)
    # Add layers
    if BLUR:
        photo = Image.alpha_composite(photo, blur)
    out = Image.alpha_composite(photo, txt)
    newpath = os.path.join(kNewDir, path)
    out.save(newpath)
