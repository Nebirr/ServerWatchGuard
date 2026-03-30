#include <Windows.h>
#include <TlHelp32.h>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>

SERVICE_STATUS			serviceStatus	= { 0 };
SERVICE_STATUS_HANDLE	statusHandle = NULL;
bool					bRunning = true;
FILETIME g_lastWriteTime = { 0 };

struct WatchItem {
	std::wstring name;
	std::wstring path;
	std::wstring dir;
	int restartAttempts;
};

void WINAPI ServiceHandler(DWORD controlCode) {
	switch (controlCode) {
	case SERVICE_CONTROL_STOP:
		bRunning = false;
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(statusHandle, &serviceStatus);
		break;
	default:
		break;
	}
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

std::vector<WatchItem> g_WatchList;

void LoadConfig() {
	g_WatchList.clear();
	std::wstring configFile = L"C:\\WatchLogs\\config.ini";

	int count = GetPrivateProfileIntW(L"Settings", L"Count", 0, configFile.c_str());

	for (int i = 1; i <= count; i++) {
		wchar_t name[256], path[MAX_PATH], dir[MAX_PATH];
		std::wstring section = L"Process" + std::to_wstring(i);

		GetPrivateProfileStringW(section.c_str(), L"Name", L"", name, 256, configFile.c_str());
		GetPrivateProfileStringW(section.c_str(), L"Path", L"", path, MAX_PATH, configFile.c_str());
		GetPrivateProfileStringW(section.c_str(), L"Dir", L"", dir, MAX_PATH, configFile.c_str());

		WatchItem item;
		item.name = name;
		item.path = path;
		item.dir = dir;
		item.restartAttempts = 0;
		g_WatchList.push_back(item);
	}

	WriteToLog("Config loaded: " + std::to_string(g_WatchList.size()) + " Processes are monitored.");
}

void StartTargetProcess(std::wstring fullPath, std::wstring workingDir) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	std::vector<wchar_t> pathBuf(fullPath.begin(), fullPath.end());
	pathBuf.push_back(L'\0');

	if (CreateProcessW(
		NULL,               
		pathBuf.data(),     
		NULL,               
		NULL,               
		FALSE,              
		0,                  
		NULL,               
		workingDir.c_str(), 
		&si,                
		&pi                 
	))
	{
		WriteToLog("SUCCESS: Process started successfully.");

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else {
		DWORD error = GetLastError();
		WriteToLog("FATAL: Could not start process! Error Code: " + std::to_string(error));
	}
}

void WINAPI ServiceMain(DWORD argc, LPTSTR* argv) {

	LoadConfig();

	statusHandle = RegisterServiceCtrlHandler(L"ServerWatchGuard", ServiceHandler);

	serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	SetServiceStatus(statusHandle, &serviceStatus);

	WriteToLog("Service started ...");

	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(statusHandle, &serviceStatus);

	WriteToLog("Service is activ.");

	while (bRunning) {
		WIN32_FILE_ATTRIBUTE_DATA data;
		if (GetFileAttributesExW(L"C:\\WatchLogs\\config.ini", GetFileExInfoStandard, &data)) {
			if (CompareFileTime(&data.ftLastWriteTime, &g_lastWriteTime) != 0) {
				g_lastWriteTime = data.ftLastWriteTime; 
				WriteToLog("Detected config change! Reloading...");
				LoadConfig();
			}
		}
		for (auto& item : g_WatchList) {

			if (IsProcessRunning(item.name)) {
				if (item.restartAttempts > 0) {
					WriteToLog("INFO: " + std::string(item.name.begin(), item.name.end()) + " is back online. Resetting counter.");
					item.restartAttempts = 0;
				}
			}
			else {
				std::string nameStr(item.name.begin(), item.name.end());

				if (item.restartAttempts < 3) {
					item.restartAttempts++;
					WriteToLog("ALARM: " + nameStr + " missing! Attempt " + std::to_string(item.restartAttempts) + " of 3...");
					StartTargetProcess(item.path, item.dir);
				}
				else if (item.restartAttempts == 3) {
					WriteToLog("CRITICAL: Max restart attempts reached for " + nameStr + ". Stopping further attempts to save resources.");
					item.restartAttempts++; 
				}
				
			}
		}
		Sleep(10000);
	}
	WriteToLog("Service is shuting down safely.");
}

int main() {
	SERVICE_TABLE_ENTRY serviceTable[] = {
		{(LPWSTR)L"ServerWatchGuard", (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};

	StartServiceCtrlDispatcher(serviceTable);

	return 0;
}

