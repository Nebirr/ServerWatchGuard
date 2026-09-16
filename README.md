# 🛡️ ServerWatchGuard (v1.3)

**ServerWatchGuard** is a robust Windows Service written in C++, designed to monitor, automatically restart, and report the status of server processes (e.g., Valheim, Arma 3, Nginx, Enshrouded) with high reliability and visual flair.

---

## ✨ Key Features
- **Native Windows Service:** Runs silently in the background (Session 0). Starts automatically with Windows—no open console windows needed.
- **Local Desktop Mode (Hybrid Execution):** Double-click the `.exe` to run the tool as a standard application in your active session. Includes a fully functional **System Tray Icon** with a right-click menu to toggle the console, open logs, edit configs, and quit the application safely.
- **Smart Auto-Installation (Local Mode):** Automatically copies itself to `C:\WatchLogs`, sets up a Windows Autostart entry, and creates default configuration files on the first launch.
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
WebhookURL=[https://discord.com/api/webhooks/YOUR_WEBHOOK_ID/YOUR_TOKEN](https://discord.com/api/webhooks/YOUR_WEBHOOK_ID/YOUR_TOKEN)

[Process1]
Name=server_process.exe
DisplayName=My Game Server
IconURL=[https://your-domain.com/icons/server-icon.png](https://your-domain.com/icons/server-icon.png)
Path=C:\Apps\MyServer\server_process.exe
Dir=C:\Apps\MyServer\
LogPath=C:\Apps\MyServer\logs\latest.log
LogSearch=Started session
```
---

## 🚀 Installation & Setup

**Option A: Run as a Windows Service (For True Servers)**

*Manual setup is required to register the application as a Session 0 background service.*

- Move the `WatchGuardService.exe`  to `C:\WatchLogs\.`

- Edit your `config.ini` in the same directory.

- Open `CMD` (Command Prompt) as Administrator and register the service:

- `sc create ServerWatchGuard binPath= "C:\WatchLogs\WatchGuardService.exe" start= auto`

*(Note: Ensure there is a space after binPath= and start=)*

- Start the service: `sc start ServerWatchGuard`

- Stop the service: `sc stop ServerWatchGuard`

- Delete the service: `sc delete ServerWatchGuard`

**Option B: Run in Local Mode (For Desktop & Testing)**

*Automated setup via double-click (Hybrid Execution).*

- Simply double-click the `WatchGuardService.exe` from anywhere (e.g., your Downloads folder).

- The application will ask if you want to add it to your Windows Autostart.

- It will automatically move itself to `C:\WatchLogs\`, generate the `config_local.ini`, and restart itself in Desktop Mode.

- Look for the WatchGuard icon in your System Tray (bottom right of your screen). Right-click it to manage the tool, edit configs, or open logs!
---
## 📜 History
ServerWatchGuard is the official successor to the ServerChecker project, evolving from a simple console tool into a professional-grade system service.

---
