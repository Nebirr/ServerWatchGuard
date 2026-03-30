# 🛡️ ServerWatchGuard

A high-performance Windows Service designed to monitor and automatically restart server processes (e.g., Valheim, Nginx, Game Servers).

## ✨ Key Features
* **Native Windows Service:** Runs silently in the background (Session 0). Starts automatically with Windows—no open console windows needed.
* **Smart Monitoring:** Scans the process list every 10 seconds with minimal CPU impact.
* **Auto-Recovery:** Detects crashes and restarts processes in their native working directory (fixing common path/config issues).
* **Hot-Reload:** Automatically detects changes in `config.ini` and reloads settings without service interruption.
* **Safety Threshold:** Limits restart attempts to 3 per cycle to prevent resource exhaustion in case of fatal path errors.

## 🚀 Installation & Setup
1. Download the latest `WatchGuardService.exe` from the [Releases](INSERT_LINK_TO_RELEASES_LATER) section.
2. Create the folder `C:\WatchLogs\` and place a `config.ini` inside.
3. Open CMD (Command Prompt) as **Administrator** and run:
   ```cmd
   sc create ServerWatchGuard binPath= "C:\Path\To\WatchGuardService.exe" start= auto
   sc start ServerWatchGuard

⚙️ Configuration (config.ini)

[Settings]
Count=1

[Process1]
Name=valheim_server.exe
Path=C:\Servers\Valheim\valheim_server.exe
Dir=C:\Servers\Valheim\

📜 History
ServerWatchGuard is the official successor to the original [ServerChecker] project, evolving from a simple console tool into a robust system service.
