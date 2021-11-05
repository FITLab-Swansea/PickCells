# Setting up PickCells

Follow these steps:
1. Open `PickCells/PickCells.pro` file with QtCreator
    - Download QtCreator from [qt.io](https://www.qt.io/download)
        - Check for an educational license
        - Otherwise use the open-source online installer
    - Note: This project was built running Qt v4.10.2. That version may not be compatible with your OS.
1. Launch Node.js server
    - `cd Server/` -> navigate to Server directory
    - `npm install` -> install node packages
    - `node server` -> run server
1. Set up a WiFi router and connect to it from your laptop, forcing the laptop's IP to be 192.168.1.100
    - Change in device preferences
1. Connect the watches to the WiFi router (the server should fire logs for every watch connected)
    - Didn't see logs
1. Launch the Android app on the watch (make sure to pair the Bluetooth module)
1. In the Qt app, press c to connect to the server (the server should fire a log message)