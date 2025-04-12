import os
from PIL import Image, ImageDraw

# Define the icon size
size = 24
padding = 2
icon_size = size - (padding * 2)

# Create icons directory if it doesn't exist
if not os.path.exists('icons'):
    os.makedirs('icons')

# Home icon
img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)
# Simple house shape
draw.polygon([(padding, size//2), (size//2, padding), (size-padding, size//2), (size-padding, size-padding), (padding, size-padding)], 
             fill=(98, 0, 238, 255))  # Material purple #6200EE
img.save('icons/home.png')

# Code icon
img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)
# Code brackets
draw.line([(padding, padding), (padding+icon_size//3, padding)], fill=(98, 0, 238, 255), width=2)
draw.line([(padding, padding), (padding, size-padding)], fill=(98, 0, 238, 255), width=2)
draw.line([(padding, size-padding), (padding+icon_size//3, size-padding)], fill=(98, 0, 238, 255), width=2)
draw.line([(size-padding-icon_size//3, padding), (size-padding, padding)], fill=(98, 0, 238, 255), width=2)
draw.line([(size-padding, padding), (size-padding, size-padding)], fill=(98, 0, 238, 255), width=2)
draw.line([(size-padding, size-padding), (size-padding-icon_size//3, size-padding)], fill=(98, 0, 238, 255), width=2)
img.save('icons/code.png')

# Lock icon
img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)
# Padlock shape
draw.rectangle([padding+2, size//2, size-padding-2, size-padding], fill=(98, 0, 238, 255))
draw.arc([padding+4, padding, size-padding-4, size//2+2], 0, 180, fill=(98, 0, 238, 255), width=2)
img.save('icons/lock.png')

# Settings icon
img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)
# Gear shape
center = size // 2
radius = icon_size // 3
outer_radius = icon_size // 2
for i in range(8):
    angle = i * (360 // 8)
    angle_rad = angle * 3.14159 / 180
    x1 = center + int(radius * -1 * 0.8 * (0.5 - 0.5 * (angle // 45 % 2)))
    y1 = center + int(radius * 0.8 * (0.5 - 0.5 * (angle // 45 % 2)))
    x2 = center + int(outer_radius * -1)
    y2 = center + int(outer_radius)
    draw.line([(center, center), (x2, y2)], fill=(98, 0, 238, 255), width=2)
draw.ellipse([center-radius//2, center-radius//2, center+radius//2, center+radius//2], fill=(98, 0, 238, 255))
img.save('icons/settings.png')

# Message icon
img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)
# Chat bubble
draw.rounded_rectangle([padding, padding, size-padding, size-padding-4], radius=3, fill=(98, 0, 238, 255))
# Triangle for the speech bubble
draw.polygon([(size-padding-8, size-padding-4), (size-padding-4, size-padding+2), (size-padding-12, size-padding-4)], 
             fill=(98, 0, 238, 255))
img.save('icons/message.png')

print("Icons created successfully in 'icons' directory.") 