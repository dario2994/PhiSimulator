convert images/*.pbm images/%05d.jpg
mogrify -resize 800x800  images/*.jpg
avconv -r 24 -qscale 2  -i images/%05d.jpg orbita.mp4
rm images/*
