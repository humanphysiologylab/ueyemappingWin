Download notes:
You only need these files to use this program:
recorder/recorder.exe
recorder/config.txt
recorder/ueye_converter.py
Don't download the whole repository if you don't plan on modifying it.

Installation notes:
*if you have already installed ueye camera on this system, proceed to step 6*
1.If you don't have account on https://en.ids-imaging.com , create one, it's free and doesn't require serial numbers
2.Log in and download the latest driver from https://en.ids-imaging.com/download-ueye-win64.html . You only need Batch installer (~80 MB)
3.Extract all the files from downloaded archive into any location and launch the .exe file
4.Type in "2" (the option to install just usb drivers) and press Enter. The installer doesn't print anything and closes when it's done.
5.Connect the camera to any usb 3.0 port (marked with blue connector on older machines). Windows should begin setting it up; it may take about 10 minutes.
6.When the LED on the camera stops blinking in red and lights up in green, launch recorder.exe.
if the LED doesn't light up in green, either camera is not set up properly or it is not connected to usb 3.0 port.

Using recorder app (recorder.exe):
0.Set desired sequence length in ms in config.txt.
1.The area in the center is the central 120*120 area of camera view, which is going to be recorded.
2.The "Start Capture" button starts recording a sequence.
3.Recorded sequence appears in the "output" folder and have a timestamp as their filename.
4."*.bin" files are supported by Rhythm-1.2
