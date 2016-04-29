cd C:\Users\Daniel\AppData\Local\Google\Chrome\Application\
set freq=876


:openurl
chrome http://www.emetteurs.ch/list_freq.php?Frequence=%freq:~0,-1%.%freq:~-1%

:increasefreq
set /a freq=%freq%+1

if NOT "%freq%" == "1080" goto openurl

