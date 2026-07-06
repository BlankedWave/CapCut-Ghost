#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <set>
#include <filesystem>
#ifdef min
#undef min
#endif
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace fs = std::filesystem;

/*

    ,o888888o.           .8.          8 888888888o       ,o8BY8VTo.    8 8888      88 8888888 8888888888
   8888     `88.        .888.         8 8888    `88.    8888     `88.  8 8888      88       8 8888
,8 8888       `8.      :88888.        8 8888     `88 ,8 8888       `8. 8 8888      88       8 8888
88 8888               . `88888.       8 8888     ,88 88 8888           8 8888      88       8 8888
88 8888              .8. `88888.      8 8888.   ,88' 88 8888           8 8888      88       8 8888
88 8888             .8`8. `88888.     8 8BY8VT888P'  88 8888           8 8888      88       8 8888
88 8888            .8' `8. `88888.    8 8888         88 8888           8 8888      88       8 8888
`8 8888       .8' .8'   `8. `88888.   8 8888         `8 8888       .8' ` 8888     ,8P       8 8888
   8888     ,88' .888888888. `88888.  8 8888            8888     ,88'    8888   ,d8P        8 8888
    `8888888P'  .8'       `8. `88888. 8 8888             `8888888P'       `Y88888P'         8 8888

    ,o888888o.    8 888888888o.            .8.           ,o888888o.    8 8888     ,88'
   8888     `88.  8 8888    `88.          .888.         8888     `88.  8 8888    ,88'
,8 88B8       `8. 8 8888     `88         :88888.     ,8 8888       `8. 8 8888   ,88'
88 88Y8           8 8888     ,88        . `88888.    88 8888           8 8888  ,88'
88 8888           8 8888.   ,88'       .8. `88888.   88 8888           8 888B ,88'
88 88V8           8 888888888P'       .8`8. `88888.  88 8888           8 888Y 88'
88 88T8           8 8888`8b          .8' `8. `88888. 88 8888           8 888888<
`8 8888       .8' 8 8888 `8b.       .8'   `8. `88888.`8 8888       .8' 8 888V `Y8.
   8888     ,88'  8 8888   `8b.    .888888888. `88888.  8888     ,88'  8 888T   `Y8.
    `8888888P'    8 8888     `88. .8'       `8. `88888.  `8888888P'    8 8888     `Y8.


d8888b. db    db      db    db d888888b
88  `8D `8b  d8'      88    88 `~~88~~'
88oooY'  `8bd8'       Y8    8P    88
88~~~b.    88         `8b  d8'    88
88   8D    88          `8bd8'     88
Y8888P'    YP            YP       YP

*/

std::vector<DWORD> findProgramProcesses(const std::wstring& programName) {
    std::vector<DWORD> pids;
    PROCESSENTRY32 pe;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) return pids;

    pe.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hProcessSnap, &pe)) {
        do {
            if (programName == pe.szExeFile) {
                pids.push_back(pe.th32ProcessID);
            }
        } while (Process32Next(hProcessSnap, &pe));
    }
    CloseHandle(hProcessSnap);
    return pids;
}

void forceCloseProcesses(const std::vector<DWORD>& pids) {
    for (DWORD pid : pids) {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (hProcess) {
            TerminateProcess(hProcess, 0);
            WaitForSingleObject(hProcess, INFINITE);
            CloseHandle(hProcess);
        }
    }
}

std::wstring getExePathFromPid(DWORD pid) {
    wchar_t path[MAX_PATH];
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (hProcess != NULL) {
        DWORD length = MAX_PATH;
        if (QueryFullProcessImageName(hProcess, 0, path, &length)) {
            CloseHandle(hProcess);
            return std::wstring(path, length);
        }
        else {
            std::cerr << "Failed to get process path for PID " << pid << std::endl;
        }
        CloseHandle(hProcess);
    }
    else {
        std::cerr << "Failed to open process with PID " << pid << std::endl;
    }

    return L"";
}

void binaryReplace(std::vector<char>& data, const std::vector<char>& from, const std::vector<char>& to) {
    size_t pos = 0;
    while ((pos = std::search(data.begin() + pos, data.end(), from.begin(), from.end()) - data.begin()) != data.size()) {
        data.erase(data.begin() + pos, data.begin() + pos + from.size());
        data.insert(data.begin() + pos, to.begin(), to.end());
        pos += to.size();
    }
}

void saveEditedFile(const fs::path& outputPath, const std::vector<char>& content) {
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Failed to open the output file for writing: " + outputPath.string());
    }
    outputFile.write(content.data(), content.size());
    outputFile.close();
}

void createToggledDllFiles(const fs::path& dllPath, const std::vector<char>& originalContent) {
    std::cout << "Editing files..." << std::endl;

    std::vector<char> onContent = originalContent;
    std::vector<char> offContent = originalContent;
    std::cout << "Making cracked..." << std::endl;
    binaryReplace(onContent, { '\x00', 'v', 'i', 'p', '_', 'e', 'n', 't', 'r', 'a', 'n', 'c', 'e', '\x00' },
        { '\x00', 'p', 'r', 'o', '_', 'f', 'o', 'r', 't', 'n', 'i', 't', 'e', '\x00' });

    std::cout << "Making uncracked..." << std::endl;
    binaryReplace(offContent, { '\x00', 'p', 'r', 'o', '_', 'f', 'o', 'r', 't', 'n', 'i', 't', 'e', '\x00' },
        { '\x00', 'v', 'i', 'p', '_', 'e', 'n', 't', 'r', 'a', 'n', 'c', 'e', '\x00' });

    saveEditedFile(dllPath.string() + "_On.dll", onContent);
    saveEditedFile(dllPath.string() + "_Off.dll", offContent);

    std::cout << "Files created successfully." << std::endl;
}

void editDllFile(const fs::path& dllPath, const std::string& toggleProProperties) {
    try {
        fs::path onPath = dllPath.string() + "_On.dll";
        fs::path offPath = dllPath.string() + "_Off.dll";

        if (fs::exists(onPath) && fs::exists(offPath)) {
            fs::path selectedFile = (toggleProProperties == "on") ? onPath : offPath;
            fs::copy_file(selectedFile, dllPath, fs::copy_options::overwrite_existing);
            std::cout << "Changes applied successfully." << std::endl;
            return;
        }

        std::cout << "cracked and uncracked files not found. Creating them...\nReading File..." << std::endl;

        std::ifstream dllFile(dllPath, std::ios::binary);
        if (!dllFile) {
            throw std::runtime_error("Failed to open the DLL file for reading.");
        }

        std::vector<char> dllContent((std::istreambuf_iterator<char>(dllFile)), std::istreambuf_iterator<char>());
        dllFile.close();

        createToggledDllFiles(dllPath, dllContent);

        fs::remove(dllPath);

        fs::path selectedFile = (toggleProProperties == "on") ? onPath : offPath;
        fs::copy_file(selectedFile, dllPath, fs::copy_options::overwrite_existing);

        std::cout << "Changes applied successfully." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred while editing the DLL file: " << e.what() << std::endl;
    }
}

int main() {
    std::wstring programName = L"CapCut.exe";
    std::string toggleproproperties;
    const char* logo = R""""(    ,o888888o.           .8.          8 888888888o       ,o8BY8VTo.    8 8888      88 8888888 8888888888 
   8888     `88.        .888.         8 8888    `88.    8888     `88.  8 8888      88       8 8888       
,8 8888       `8.      :88888.        8 8888     `88 ,8 8888       `8. 8 8888      88       8 8888       
88 8888               . `88888.       8 8888     ,88 88 8888           8 8888      88       8 8888       
88 8888              .8. `88888.      8 8888.   ,88' 88 8888           8 8888      88       8 8888       
88 8888             .8`8. `88888.     8 8BY8VT888P'  88 8888           8 8888      88       8 8888       
88 8888            .8' `8. `88888.    8 8888         88 8888           8 8888      88       8 8888       
`8 8888       .8' .8'   `8. `88888.   8 8888         `8 8888       .8' ` 8888     ,8P       8 8888       
   8888     ,88' .888888888. `88888.  8 8888            8888     ,88'    8888   ,d8P        8 8888       
    `8888888P'  .8'       `8. `88888. 8 8888             `8888888P'       `Y88888P'         8 8888       
                                                                                                         
    ,o888888o.    8 888888888o.            .8.           ,o888888o.    8 8888     ,88'                   
   8888     `88.  8 8888    `88.          .888.         8888     `88.  8 8888    ,88'                    
,8 88B8       `8. 8 8888     `88         :88888.     ,8 8888       `8. 8 8888   ,88'                     
88 88Y8           8 8888     ,88        . `88888.    88 8888           8 8888  ,88'                      
88 8888           8 8888.   ,88'       .8. `88888.   88 8888           8 888B ,88'                       
88 88V8           8 888888888P'       .8`8. `88888.  88 8888           8 888Y 88'                        
88 88T8           8 8888`8b          .8' `8. `88888. 88 8888           8 888888<                         
`8 8888       .8' 8 8888 `8b.       .8'   `8. `88888.`8 8888       .8' 8 888V `Y8.                       
   8888     ,88'  8 8888   `8b.    .888888888. `88888.  8888     ,88'  8 888T   `Y8.                     
    `8888888P'    8 8888     `88. .8'       `8. `88888.  `8888888P'    8 8888     `Y8.)"""";
    std::cout << logo << std::endl;
    std::string watermark = R""""(d8888b. db    db      db    db d888888b 
88  `8D `8b  d8'      88    88 `~~88~~' 
88oooY'  `8bd8'       Y8    8P    88    
88~~~b.    88         `8b  d8'    88    
88   8D    88          `8bd8'     88    
Y8888P'    YP            YP       YP)"""";
    std::cout << "" << watermark << "\nType \"on\" or \"off\" (CASE SENSITIVE):" << std::endl;
    std::cin >> toggleproproperties;

    auto capcutPids = findProgramProcesses(programName);
    std::set<fs::path> dllPaths;

    if (!capcutPids.empty()) {
        for (DWORD pid : capcutPids) {
            std::wstring exePath = getExePathFromPid(pid);
            if (!exePath.empty()) {
                fs::path exeFolderPath = fs::path(exePath).parent_path();
                fs::path dllPath = exeFolderPath / "VECreator.dll";
                fs::path watermarkFolderPath = exeFolderPath / "Resources" / "watermark";

                if (fs::exists(dllPath)) {
                    dllPaths.insert(dllPath);
                }
                if (fs::exists(watermarkFolderPath) && fs::is_directory(watermarkFolderPath)) {
                    std::error_code ec;
                    fs::remove_all(watermarkFolderPath, ec);
                    if (!ec) {
                        std::cout << "Deleted watermarks. " << std::endl;
                    }
                    else {
                        std::cerr << "Failed to delete watermark folder: " << watermarkFolderPath
                            << ". Error: " << ec.message() << std::endl;
                    }
                }
            }
        }

        forceCloseProcesses(capcutPids);

        std::cout << "Waiting for all instances to close completely..." << std::endl;
        while (!findProgramProcesses(programName).empty()) {
            Sleep(100);
        }
        std::cout << "All instances of CapCut have been closed." << std::endl;

        for (const auto& dllPath : dllPaths) {
            editDllFile(dllPath, toggleproproperties);
        }
    }
    else {
        std::cout << "No instances of CapCut.exe were found." << std::endl;
    }

    std::cout << "Finished. Press any key to close." << std::endl;
    std::cin.ignore();
    std::cin.get();
    return 0;
}
