@echo off
:start
netsh wlan start hostednetwork
cls
netsh wlan show hostednetwork
timeout 60
goto start

