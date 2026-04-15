#include <Windows.h>
#include <TlHelp32.h>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>

SERVICE_STATUS serviceStatus = { 0 };
SERVICE_STATUS_HANDLE statusHandle = NULL;
bool bRunning = true;
FILETIME g_lastWriteTime = { 0 };
std::wstring g_webPath = L"NONE";
std::wstring g_webhookURL = L"NONE";

struct WatchItem {
    std::wstring name;
    std::wstring displayName;
    std::wstring iconURL;
    std::wstring path;
    std::wstring dir;
    std::wstring logPath;
    std::wstring logSearch;
    int restartAttempts;
};

std::vector<WatchItem> g_WatchList;

std::string wstringToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

void WriteToLog(std::string message) {
    std::ofstream logFile("C:\\WatchLogs\\ServerWatchGuard.log", std::ios::app);
    if (logFile.is_open()) {
        std::time_t now = std::time(0);
        char timestamp[26];
        ctime_s(timestamp, sizeof(timestamp), &now);
        std::string timeStr(timestamp);
        if (!timeStr.empty()) timeStr.pop_back();
        logFile << "[" << timeStr << "] " << message << std::endl;
        logFile.close();
    }
}

std::wstring extrahiereLogWert(std::wstring logPath, std::wstring sucheW) {
    if (logPath == L"NONE" || sucheW == L"NONE" || logPath.empty()) return L"";
    std::string suche = wstringToUtf8(sucheW);
    std::ifstream file(logPath);
    std::string line;
    std::string gefundenesResultat = "";
    if (file.is_open()) {
        while (std::getline(file, line)) {
            size_t pos = line.find(suche);
            if (pos != std::string::npos) {
                gefundenesResultat = line.substr(pos + suche.length());
                gefundenesResultat.erase(gefundenesResultat.find_last_not_of(" \n\r\t") + 1);
            }
        }
        file.close();
    }
    return std::wstring(gefundenesResultat.begin(), gefundenesResultat.end());
}

void SendDiscordNotification(std::wstring displayTitle, bool istAbsturz, std::wstring zusatzInfo = L"", std::wstring thumbnailURL = L"") {
    if (g_webhookURL == L"NONE" || g_webhookURL.empty()) return;

    std::wstring webhookURL = g_webhookURL;
    std::wstring finalIcon = thumbnailURL;
    if (finalIcon.length() < 10) finalIcon = L"https://nebirrs-lab.de/Lab_Logo_apple.png";

    std::wstring emoji = istAbsturz ? L"$([char]0x26A0)" : L"$([char]0x2705)";
    std::wstring statusPrefix = istAbsturz ? L" Alarm: " : L" Recovery: ";
    std::wstring wFarbe = istAbsturz ? L"15548997" : L"3066993";

    std::wstring psCommand = L"$msg = @{ "
        L"username = 'Nebirrs Lab WatchGuard'; "
        L"embeds = @(@{ "
        L"title = " + emoji + L" + '" + statusPrefix + displayTitle + L"'; "
        L"color = " + wFarbe + L"; "
        L"thumbnail = @{ url = '" + finalIcon + L"' }; "
        L"fields = @(@{ name = 'Status'; value = '" + (istAbsturz ? L"OFFLINE" : L"ONLINE") + L"'; inline = $true }";

    if (!zusatzInfo.empty()) {
        psCommand += L", @{ name = 'Info'; value = '" + zusatzInfo + L"'; inline = $true }";
    }

    psCommand += L") }) }; "
        L"$json = $msg | ConvertTo-Json -Depth 10; "
        L"Invoke-RestMethod -Uri '" + webhookURL + L"' -Method Post -Body ([System.Text.Encoding]::UTF8.GetBytes($json)) -ContentType 'application/json; charset=utf-8'";

    std::wstring finalCommand = L"powershell.exe -ExecutionPolicy Bypass -WindowStyle Hidden -Command \"" + psCommand + L"\"";
    _wsystem(finalCommand.c_str());
}

bool IsProcessRunning(std::wstring processName) {
    bool exists = false;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(PROCESSENTRY32W);
        if (Process32FirstW(hSnapshot, &pe)) {
            do {
                if (processName == pe.szExeFile) {
                    exists = true;
                    break;
                }
            } while (Process32NextW(hSnapshot, &pe));
        }
        CloseHandle(hSnapshot);
    }
    return exists;
}

void LoadConfig() {
    g_WatchList.clear();
    std::wstring configFile = L"C:\\WatchLogs\\config.ini";

    wchar_t wPath[MAX_PATH];
    GetPrivateProfileStringW(L"Settings", L"WebPath", L"NONE", wPath, MAX_PATH, configFile.c_str());
    g_webPath = wPath;
    size_t firstWeb = g_webPath.find_first_not_of(L" ");
    if (firstWeb != std::wstring::npos) g_webPath.erase(0, firstWeb);

    wchar_t wWebhook[1024];
    GetPrivateProfileStringW(L"Settings", L"WebhookURL", L"NONE", wWebhook, 1024, configFile.c_str());
    g_webhookURL = wWebhook;
    size_t firstWH = g_webhookURL.find_first_not_of(L" ");
    if (firstWH != std::wstring::npos) g_webhookURL.erase(0, firstWH);

    if (g_webhookURL != L"NONE") {
        WriteToLog("Config: Webhook loaded (Length: " + std::to_string(g_webhookURL.length()) + ")");
    }
    else {
        WriteToLog("Config: NO Webhook configured (NONE).");
    }

    int count = GetPrivateProfileIntW(L"Settings", L"Count", 0, configFile.c_str());

    for (int i = 1; i <= count; i++) {
        wchar_t name[256], path[MAX_PATH], dir[MAX_PATH], lPath[MAX_PATH], lSearch[256], dName[256], iURL[512];
        std::wstring section = L"Process" + std::to_wstring(i);

        GetPrivateProfileStringW(section.c_str(), L"Name", L"", name, 256, configFile.c_str());
        GetPrivateProfileStringW(section.c_str(), L"Path", L"", path, MAX_PATH, configFile.c_str());
        GetPrivateProfileStringW(section.c_str(), L"Dir", L"", dir, MAX_PATH, configFile.c_str());
        GetPrivateProfileStringW(section.c_str(), L"LogPath", L"NONE", lPath, MAX_PATH, configFile.c_str());
        GetPrivateProfileStringW(section.c_str(), L"LogSearch", L"NONE", lSearch, 256, configFile.c_str());
        GetPrivateProfileStringW(section.c_str(), L"DisplayName", name, dName, 256, configFile.c_str());
        GetPrivateProfileStringW(section.c_str(), L"IconURL", L"NONE", iURL, 512, configFile.c_str());

        std::wstring cleanedDName = dName;
        size_t firstD = cleanedDName.find_first_not_of(L" ");
        if (firstD != std::wstring::npos) cleanedDName.erase(0, firstD);

        std::wstring cleanedIcon = iURL;
        size_t firstI = cleanedIcon.find_first_not_of(L" ");
        if (firstI != std::wstring::npos) cleanedIcon.erase(0, firstI);
        if (cleanedIcon == L"NONE") cleanedIcon = L"";

        WatchItem item;
        item.name = name;
        item.displayName = cleanedDName;
        item.iconURL = cleanedIcon;
        item.path = path;
        item.dir = dir;
        item.logPath = lPath;
        item.logSearch = lSearch;
        item.restartAttempts = 0;
        g_WatchList.push_back(item);
    }
    WriteToLog("Config loaded: " + std::to_string(g_WatchList.size()) + " Processes monitored.");
}

void StartTargetProcess(std::wstring fullPath, std::wstring workingDir) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::vector<wchar_t> pathBuf(fullPath.begin(), fullPath.end());
    pathBuf.push_back(L'\0');

    if (CreateProcessW(NULL, pathBuf.data(), NULL, NULL, FALSE, 0, NULL, workingDir.c_str(), &si, &pi)) {
        WriteToLog("SUCCESS: Process started.");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        WriteToLog("FATAL: Could not start process! Error: " + std::to_string(GetLastError()));
    }
}

void UpdateWebsiteStatus() {
    if (g_webPath == L"NONE" || g_webPath.empty()) return;

    std::ofstream jsonFile(g_webPath);
    if (jsonFile.is_open()) {
        jsonFile << "{";
        for (size_t i = 0; i < g_WatchList.size(); ++i) {
            bool aktiv = IsProcessRunning(g_WatchList[i].name);
            jsonFile << "\"" << wstringToUtf8(g_WatchList[i].name) << "\": \"" << (aktiv ? "Active" : "Offline") << "\"";
            if (i < g_WatchList.size() - 1) jsonFile << ", ";
        }
        jsonFile << "}";
        jsonFile.close();
    }
}

void WINAPI ServiceHandler(DWORD controlCode) {
    switch (controlCode) {
    case SERVICE_CONTROL_STOP:
        bRunning = false;
        serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(statusHandle, &serviceStatus);
        break;
    default:
        break;
    }
}

void WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {
    statusHandle = RegisterServiceCtrlHandler(L"ServerWatchGuard", ServiceHandler);
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    SetServiceStatus(statusHandle, &serviceStatus);

    LoadConfig();

    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(statusHandle, &serviceStatus);
    WriteToLog("Service is active.");

    SendDiscordNotification(L"SYSTEM", false, L"WatchGuard Service started!");

    while (bRunning) {
        WIN32_FILE_ATTRIBUTE_DATA data;
        if (GetFileAttributesExW(L"C:\\WatchLogs\\config.ini", GetFileExInfoStandard, &data)) {
            if (CompareFileTime(&data.ftLastWriteTime, &g_lastWriteTime) != 0) {
                g_lastWriteTime = data.ftLastWriteTime;
                WriteToLog("Config change detected! Reloading...");
                LoadConfig();
            }
        }

        UpdateWebsiteStatus();

        for (auto& item : g_WatchList) {
            if (!IsProcessRunning(item.name)) {
                if (item.restartAttempts < 3) {
                    item.restartAttempts++;
                    WriteToLog("ALARM: " + wstringToUtf8(item.name) + " missing!");

                    SendDiscordNotification(item.displayName, true, L"Crashed! Restarting...", item.iconURL);
                    StartTargetProcess(item.path, item.dir);

                    Sleep(10000);
                    std::wstring logWert = extrahiereLogWert(item.logPath, item.logSearch);
                    SendDiscordNotification(item.displayName, false, logWert.empty() ? L"Online" : (item.logSearch + L": " + logWert), item.iconURL);
                }
                else if (item.restartAttempts == 3) {
                    WriteToLog("CRITICAL: Max restarts for " + wstringToUtf8(item.name));
                    SendDiscordNotification(item.displayName, true, L"MAX RESTARTS REACHED!", item.iconURL);
                    item.restartAttempts++;
                }
            }
            else {
                item.restartAttempts = 0;
            }
        }
        Sleep(10000);
    }
    serviceStatus.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(statusHandle, &serviceStatus);
}

int main() {
    SERVICE_TABLE_ENTRY serviceTable[] = {
        {(LPWSTR)L"ServerWatchGuard", (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL, NULL}
    };
    StartServiceCtrlDispatcher(serviceTable);
    return 0;
}