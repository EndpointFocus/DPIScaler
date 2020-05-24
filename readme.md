# DPI Scaler
The `DPI Scaler` is a command line utility used to set the `Scale and Layout` DPI in Windows 10.

Windows 10 will attempt to set the optimal DPI scaling setting based on the screen size and resolution of the target device, and often sets it too aggressively. Unfortunately, Microsoft does not provide a native utility (or PowerShell cmdlet) to programmatically set the DPI scale, which is why we've written this CLI tool.

![Windows 10 Scale and Layout Settings](img/win10-dpiscaling.png?raw=true "Windows 10 Scale and Layout Settings")

## Usage
Run `DPIScaler.exe -Help` to get a list of available options
```
DPIScaler.exe -Help

Usage: DPIScaler[-GetDPIValue | -SetDPIValue | -GetAdapterID][-DisplayAdapter][-DPIValue][-Help]

Options:
    -GetDPIValue      Gets the DPI scale value of the selected monitor
    -SetDPIValue      Sets the DPI scale value of the selected monitor
    -GetAdapterID     Gets a list of display adapters
    -DisplayAdapter   Specify the display adapter (0 is usually the primary display)
    -DPIValue         Set the desired DPI scale value
    -Help             Shows the help menu

Example: DPIScaler.exe -SetDPIValue -DisplayAdapter=0 -DPIValue=125
```
## Examples
To get the current adapter ID, run `DPIScaler.exe -GetAdapterID` 
```
C:\Program Files (x86)\Endpoint Focus\DPIScaler>DPIScaler.exe -GetAdapterID
0. (internal display)
```

To get the current DPI scale for the device, run `DPIScaler.exe -DisplayAdapter=0 -GetDPIVale`
```
C:\Program Files (x86)\Endpoint Focus\DPIScaler>DPIScaler.exe -DisplayAdapter=0 -GetDPIValue
150
```
**Note** The value returned is a percentage

To set the DPI scale to 125%, run `DPIScaler.exe -SetDPIValue -DisplayAdapter=0 -DPIValue=125`

## Deployment
The utility has been packaged as an MSI and can be installed as an Application during a Windows 10 deployment

This utility can be executed at first user logon by utilising the `RunOnce` registry key as follows:
```
New-ItemProperty -Path "HKCU:\SOFTWARE\Microsoft\Windows\CurrentVersion\RunOnce" -Name "DPIScaler" -Value "cmd.exe /c `"${env:ProgramFiles(x86)}\Endpoint Focus\DPIScaler\DPIScaler.exe`" -SetDPIValue -DisplayAdapter=0 -DPIValue=125" -Type "STRING"
```

## License
This code is copyright (c) 2020 [Endpoint Focus](https://endpointfocus.com) and released under GPL 2.0 license. All rights not explicitly granted in the GPL 2.0 license are reserved. See the included LICENSE.md file for more details.