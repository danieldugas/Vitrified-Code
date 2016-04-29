@echo off
::set /p key= Input WPA Key:
netsh wlan set hostednetwork mode=allow ssid=Flux key=oneinternetplease
netsh wlan start hostednetwork
0<nul (
start /wait "C:\Users\daniel\SkyDrive\Files\Development\Batch Scripts\SHOWHOSTEDNETWORK.bat"
netsh wlan stop hostednetwork
exit
) 

:: Reminder: Check 'Allow other network users to connect through this computer's internet connection' 
:: in Network and Sharing Center>Change Adapter Settings>[Working Internet Connection]>Propreties>Sharing Tab 
:: (Also, uncheck 'Allow other network users to control or disable the shared internet connection')