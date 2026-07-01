# 🛡️ ServerWatchGuard (v1.1)

**ServerWatchGuard** is a robust Windows Service written in C++, designed to monitor, automatically restart, and report the status of server processes (e.g., Valheim, Arma 3, Nginx, Enshrouded) with high reliability and visual flair.

---

## ✨ Key Features
- **Native Windows Service:** Runs silently in the background (Session 0). Starts automatically with Windows—no open console windows needed.
- **PowerShell-Driven Discord Notifications:** Sends rich embeds via native PowerShell integration:
  - **Visual Alerts:** Supports Unicode emojis (⚠️, ✅) and custom colors.
  - **Modular Thumbnails:** Set unique Icon-URLs for every monitored process.
  - **Dynamic Info:** Automatically extracts data (like Join Codes) from server logs.
- **Web-Status Integration:** Generates a real-time `status.json` for live monitoring on your Nginx/Web dashboard.
- **Enhanced Security:** No hardcoded Webhook URLs. All sensitive data is stored in the local `config.ini`.
- **Hot-Reload:** Automatically detects changes in `config.ini` and updates settings without service interruption.
- **Fail-Safe Parsing:** Automatically trims leading/trailing spaces from configuration values.

---

## ⚠️ Architectural Note (Session 0)
This tool is explicitly designed as a **Windows Service** and runs in the background as `SYSTEM` in Windows **Session 0**. 

**What this means:**
* ✅ **Perfect for Headless Apps & Servers:** Dedicated servers (e.g., Conan Exiles, Enshrouded, Valheim), batch scripts, and true background tools work flawlessly and benefit from the highest system privileges.
* ❌ **Not for Desktop/GUI Applications:** Programs that strictly require a graphical user interface or an active Windows desktop environment (Session 1) — such as RGB controllers (SignalRGB), Discord clients, or standard web browsers — **cannot** be properly launched by this service. Attempting to do so will result in startup crashes (Access Denied / Error 15609) or the program running completely invisibly without a UI.

---

## ⚙️ Configuration (`C:\WatchLogs\config.ini`)

The service is fully modular. You can toggle features like Discord notifications or Web-Status by setting their values to `NONE`.

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

##🚀 Installation & Setup
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

## 📜 History
ServerWatchGuard is the official successor to the ServerChecker project, evolving from a simple console tool into a professional-grade system service.
