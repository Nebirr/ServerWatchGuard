# 🛡️ ServerWatchGuard (v1.2)

**ServerWatchGuard** is a robust Windows Service written in C++, designed to monitor, automatically restart, and report the status of server processes (e.g., Valheim, Arma 3, Nginx, Enshrouded) with high reliability and visual flair.

---

## ✨ Key Features
- **Native Windows Service:** Runs silently in the background (Session 0). Starts automatically with Windows—no open console windows needed.
- **Local Desktop Mode (`-local`):** Run the tool as a standard application in your active session. Includes a fully functional **System Tray Icon** with a right-click menu to toggle the console, open logs, edit configs, and quit the application safely.
- **PowerShell-Driven Discord Notifications:** Sends rich embeds via native PowerShell integration:
  - **Visual Alerts:** Supports Unicode emojis (⚠️, ✅) and custom colors.
  - **Modular Thumbnails:** Set unique Icon-URLs for every monitored process.
  - **Dynamic Info:** Automatically extracts data (like Join Codes) from server logs.
- **Web-Status Integration:** Generates a real-time `status.json` for live monitoring on your Nginx/Web dashboard.
- **Enhanced Security:** No hardcoded Webhook URLs. All sensitive data is stored in the local `config.ini`.
- **Hot-Reload:** Automatically detects changes in the configuration file and updates settings without interruption.
- **Fail-Safe Parsing:** Automatically trims leading/trailing spaces from configuration values.

---

## ⚠️ Architectural Note (Session 0 vs. Local Mode)
By default, this tool is explicitly designed as a **Windows Service** and runs in the background as `SYSTEM` in Windows **Session 0**. 

**What this means:**
* ✅ **Perfect for Headless Apps & Servers:** Dedicated servers (e.g., Conan Exiles, Enshrouded, Valheim), batch scripts, and true background tools work flawlessly and benefit from the highest system privileges.
* ❌ **Not for Desktop/GUI Applications:** Programs that strictly require a graphical user interface or an active Windows desktop environment (Session 1) — such as RGB controllers, Discord clients, or standard web browsers — **cannot** be properly launched by the background service.

**💡 Solution for GUI Apps:** 
If you need to monitor standard desktop applications, simply run the executable via Command Prompt or a shortcut using the `-local` argument. This runs ServerWatchGuard in your active user session!

---

## ⚙️ Configuration (`C:\WatchLogs\config.ini`)

The service is fully modular. You can toggle features like Discord notifications or Web-Status by setting their values to `NONE`.

*(Note: When running with the `-local` flag, the tool automatically uses `config_local.ini` and `WatchGuard_local.log` instead, keeping your service and desktop environments neatly separated.)*

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
- Download the latest ServerWatchGuard.exe.

- Create the folder C:\WatchLogs\ and place your config.ini inside.

- Open CMD (Command Prompt) as Administrator and register the service:
```sc create ServerWatchGuard binPath= "C:\Path\To\ServerWatchGuard.exe" start= auto```
- **Start the service:**
 ```sc start ServerWatchGuard```
- **Stop the service:**
 ```sc stop ServerWatchGuard```
- **Delete the service:**
 ```sc delete ServerWatchGuard```
- (Note: Ensure there is a space after binPath= and start=).

# Option B: Run in Local Mode (For Desktop & Testing)
- Create your setup inside : ``` C:\WatchLogs\ using the filename config_local.ini. ```
- Open CMD or create a Windows Shortcut and run:
```ServerWatchGuard.exe -local```
- Look for the WatchGuard icon in your System Tray (bottom right of your screen). Right-click it to manage the tool!
---
## 📜 History
ServerWatchGuard is the official successor to the ServerChecker project, evolving from a simple console tool into a professional-grade system service.

---
