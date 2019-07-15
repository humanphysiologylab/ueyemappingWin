Installation notes:
*if you have already installed ueye camera on this system, proceed to step 6*
1.If you don't have account on https://en.ids-imaging.com , create one, it's free and doesn't require serial numbers
2.Log in and download the latest driver from https://en.ids-imaging.com/download-ueye-win64.html . You only need Batch installer (~80 MB)
3.Extract all the files from downloaded archive into any location and launch the .exe file
4.Type in "2" (the option to install just usb drivers) and press Enter. The installer doesn't print anything and closes when it's done.
5.Connect the camera to any usb 3.0 port (marked with blue connector on older machines). Windows should begin setting it up; it may take about 10 minutes.
6.When the LED on the camera stops blinking in red and lights up in green, launch IdsSimpleLive.exe.
if the LED doesn't light up in green, either camera is not set up properly or it is not connected to usb 3.0 port.

Using recorder app (IdsSimpleLive.exe):
1.The area in the center is the central 120*120 area of camera view, which is going to be recorded.
2.The "Load parameters" button starts recording a 2.5 seconds sequence
3.Recorded sequence is always named "sequence.bin" and is located in the same folder as IdsSimpleLive.exe
4.You may use the "ueye_converter.py" python script to convert sequence into .mat files, which are compatible with rhythm-1.2