# 🛡️ ServerWatchGuard (v1.4)

![C++](https://img.shields.io/badge/Language-C++-00599C?logo=c%2B%2B)
![Windows](https://img.shields.io/badge/Platform-Windows-0078D4?logo=windows)
![Release](https://img.shields.io/github/v/release/Nebirr/ServerWatchGuard)
![Last Commit](https://img.shields.io/github/last-commit/Nebirr/ServerWatchGuard)
![Issues](https://img.shields.io/github/issues/Nebirr/ServerWatchGuard)

**ServerWatchGuard** is a robust Windows Service written in C++, designed to monitor, automatically restart, and report the status of server processes (e.g., Valheim, Arma 3, Nginx, Enshrouded) with high reliability and visual flair.

---

## ✨ Key Features
- **Native Windows Service:** Runs silently in the background (Session 0). Starts automatically with Windows—no open console windows needed.
- **Local Desktop Mode (Hybrid Execution):** Double-click the `.exe` to run the tool as a standard application in your active session. Includes a fully functional **System Tray Icon** with a right-click menu to toggle the console, open logs, edit configs, and quit the application safely.
- **Smart Auto-Installation (UAC Bypass):** Automatically copies itself to `C:\WatchLogs` and sets up an elevated Windows Task Scheduler entry. This ensures the app starts perfectly with Windows in Local Mode without triggering annoying Admin (UAC) prompts on every boot.
- **Single-Instance Guard:** Prevents multiple instances from running simultaneously and provides an interactive UI prompt to safely restart existing background processes.
- **PowerShell-Driven Discord Notifications:** Sends rich embeds via native PowerShell integration:
  - **Visual Alerts:** Supports Unicode emojis (⚠️, ✅) and custom colors.
  - **Modular Thumbnails:** Set unique Icon-URLs for every monitored process.
  - **Dynamic Info:** Automatically extracts data (like Join Codes in Games like Valheim) from server logs.
- **Web-Status Integration:** Generates a real-time `status.json` for live monitoring on your Nginx/Web dashboard.
- **Hot-Reload:** Automatically detects changes in the configuration file and updates settings without interruption.
- **Fail-Safe Parsing:** Automatically trims leading/trailing spaces from configuration values.

---

## ⚠️ Architectural Note (Session 0 vs. Local Mode)
By default, this tool is explicitly designed as a **Windows Service** and runs in the background as `SYSTEM` in Windows **Session 0**. 

**What this means:**
* ✅ **Perfect for Headless Apps & Servers:** Dedicated servers (e.g., Conan Exiles, Enshrouded, Valheim), batch scripts, and true background tools work flawlessly and benefit from the highest system privileges.
* ❌ **Not for Desktop/GUI Applications:** Programs that strictly require a graphical user interface or an active Windows desktop environment (Session 1) — such as RGB controllers, Discord clients, or standard web browsers — **cannot** be properly launched by the background service.

**💡 Solution for GUI Apps:** 
If you need to monitor standard desktop applications, simply use the **Local Mode** (see Option B below). This runs ServerWatchGuard in your active user session!

---

## ⚙️ Configuration (`C:\WatchLogs\config.ini`)

The service is fully modular. You can toggle features like Discord notifications or Web-Status by setting their values to `NONE`. The tool will **automatically generate** a default config file on its first run.

*(Note: When running in local mode, the tool automatically uses `config_local.ini` and `WatchGuard_local.log` instead, keeping your service and desktop environments neatly separated.)*

```ini
[Settings]
Count=1
WebPath=C:\Path\To\Your\Webserver\status.json
WebhookURL=https://discord.com/api/webhooks/YOUR_WEBHOOK_ID/YOUR_TOKEN

[Process1]
Name=server_process.exe
DisplayName=My Game Server
IconURL=https://your-domain.com/icons/server-icon.png
Path=C:\Apps\MyServer\server_process.exe
Dir=C:\Apps\MyServer\
LogPath=C:\Apps\MyServer\logs\latest.log
LogSearch=Started session
```
---

## 🚀 Installation & Setup

### Option A: Run as a Windows Service (For True Servers)
*Manual setup is required to register the application as a Session 0 background service.*

1. Move the `WatchGuardService.exe` to `C:\WatchLogs\`.
2. Edit your `config.ini` in the same directory.
3. Open **CMD** (Command Prompt) as Administrator and register the service:
   ```cmd
   sc create ServerWatchGuard binPath= "C:\WatchLogs\WatchGuardService.exe" start= auto
   ```
   *(Note: Ensure there is a space after `binPath=` and `start=`)*
4. Start the service: 
   ```cmd
   sc start ServerWatchGuard
   ```
5. Stop the service: 
   ```cmd
   sc stop ServerWatchGuard
   ```
6. Delete the service: 
   ```cmd
   sc delete ServerWatchGuard
   ```

### Option B: Run in Local Mode (For Desktop & Testing)
*Automated setup via double-click (Hybrid Execution).*

1. Simply double-click the `WatchGuardService.exe` from anywhere (e.g., your Downloads folder). 
2. Windows will ask for Administrator privileges (required to create the automated background task).
3. The application will ask if you want to add it to your Windows Autostart.
4. It will automatically move itself to `C:\WatchLogs\`, generate the `config_local.ini`, register the silent Autostart task, and restart itself in Desktop Mode.
5. Look for the WatchGuard icon in your System Tray (bottom right of your screen). Right-click it to manage the tool, edit configs, or open logs!

---

## 📜 History
ServerWatchGuard is the official successor to the ServerChecker project, evolving from a simple console tool into a professional-grade system service.
