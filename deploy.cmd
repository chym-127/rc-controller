@echo off
@setlocal EnableDelayedExpansion

start /WAIT /b scp -r ./dist/* root@150.158.13.8:/var/www/html