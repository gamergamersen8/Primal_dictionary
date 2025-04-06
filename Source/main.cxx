#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <bitset>
#include <cstdio>
#include <memory>
#include <cstdlib>
#include <limits.h>
#include <thread>
#include <chrono>
using namespace std;

#define forloop(len) for(int i = 0; i< len; i++)

#ifdef _WIN32
#include <windows.h>
class Window {
private:
    HANDLE hPipe;
public:
    Window() : hPipe(INVALID_HANDLE_VALUE) {}
    bool WindowMain() {
        Sleep(15);

        hPipe = CreateFile(
            TEXT("\\\\.\\pipe\\PrimalDictionary"),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Could not open pipe. GLE=" << GetLastError() << std::endl;
            return FALSE;
        }
        CloseHandle(hPipe);
        return TRUE;
    }
    std::string WindowIn(bool& pipeOpen) {
        if (!pipeOpen) {
            return "Pipe is not available";
        }
        char buffer[128];
        DWORD bytesRead;
        DWORD bytesAvailable;
        while (PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvailable, NULL)) {
        }
        if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';
            return std::string(buffer);
        }
    }
    bool WindowOut(std::vector<std::string>& message) {
        DWORD BytesWritten;
        for (std::string match : message) {
            bool success = WriteFile(hPipe, match.c_str(), static_cast<DWORD>(match.length()), &BytesWritten, NULL);

            if (!success) {
                std::cerr << "Error in writing to pipe" << std::endl;
                return false;
            }
        }
        return true;
    }
    void ClosePipe() {
        if (hPipe != INVALID_HANDLE_VALUE) {
            CloseHandle(hPipe);
            hPipe = INVALID_HANDLE_VALUE;
        }
    }
    ~Window() {
        ClosePipe();
    }
};
#endif

bool PrintResult(std::vector<std::string>&matches, bool& pipeOpen) {
    if (pipeOpen) {
        Window window;
        bool result = window.WindowOut(matches);

        return result;
    }
    else {
        if (matches.size() == 1) {
            std::cout << "The translation is: " << matches[0] << std::endl;
            return true;
        }
        else if (matches.size() > 1) {
            std::cout << "Possible translations: ";
            forloop(matches.size()) {
                std::cout << "\n" << matches[i];
            }
            std::cout << std::endl;
            return true;
        }
        else {
            std::cout << "Found no translation" << std::endl;
            return true;
        }
    }
}

std::string readIn(bool& pipeOpen) {
    if (pipeOpen) {
        Window window;
        std::string read = window.WindowIn(pipeOpen);
        return read;
    }
    else {
        std::string read = "";
        std::cin >> read;
        return read;
    }
}

std::string PingFiles() {
    try {
        std::ifstream EngPri("Skyhawk English To Primal.csv");
        std::ifstream PriEng("Skyhawk Primal To English.csv");
        if (!EngPri) {
            std::cout << "Failed to open file: " << "Skyhawk English To Primal.csv" << std::endl;
            throw 56;
        }
        if (!PriEng) {
            std::cout << "Failed to open file: " << "Skyhawk Primal To English.csv" << std::endl;
            throw 56;
        }
        return "Done";
    }
    catch (int Error) {
        printf("Unable to locate csv files\n");
        try {
            std::cout << "Running python script ... ";
            std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen("Python3 ./XlsxToCsv.py", "r"), _pclose);
            if (!pipe) {
                std::cerr << "Error" << std::endl;
                throw "Error";
            }
            // Read the output of the Python script
            char buffer[128];
            std::string result;
            while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
                result += buffer;
            }
            std::cout << "Done" << std::endl;
            return result;
        }
        catch (std::string error) {
            std::cout << "Unable to locate csv files and unable to run python script" << std::endl;
            return "Error";
        }
    }
}

std::vector<std::vector<std::string>> readCSV(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> fields;
        std::string field;
        bool inQuotes = false;

        for (char c : line) {
            if (!inQuotes && c == ',') {
                fields.push_back(field);
                field.clear();
            }
            else if (c == '"') {
                inQuotes = !inQuotes;
            }
            else {
                field += c;
            }
        }
        fields.push_back(field);
        data.push_back(fields);
    }

    file.close();
    return data;
}

std::vector<std::string> PrimalToEnglish(const std::string& filename, const std::string& TBT, bool& pipeOpen) {
    const int GrammaticalType_colom = 1;
    const int Primal_colom = 2;
    const int TricksterTransscription_colom = 3;
    const int SkyhawkTransscription_colom = 4;
    const int English_colom = 5;
    auto data = readCSV(filename);
    std::vector<std::string> matches;

    bool perfectMatch = false;

    for (const auto& row : data) {
        if (row[Primal_colom] == TBT) {
            perfectMatch = true;
            matches.push_back(row[English_colom]);
            PrintResult(matches, pipeOpen);
        }
    }
    if (!perfectMatch) {
        for (const auto& row : data) {
            if (row[Primal_colom].find(TBT) < row[Primal_colom].length()) {
                matches.push_back(row[English_colom]);
            }
        }
        std::cout << matches.size();
        PrintResult(matches, pipeOpen);
    }
    return matches;
}

int main() {
    Window window;
    bool window_status = window.WindowMain();
    if (!window_status) {
        std::cout << "Unable to connect to c# file \n";
        bool pipeOpen = FALSE;
    }
    else {
        bool pipeOpen = TRUE;
    }
    bool pipeOpen = FALSE;

    std::string re = PingFiles();
    if (!re.find("Error")) {
        std::cerr << "Error in running python script";
        std::cout << re << std::endl;
        std::vector<std::string> errorVec;
        errorVec.push_back("Error check existence of csv and python files");
        PrintResult(errorVec, pipeOpen);
    }

    const std::string PTE = "Skyhawk English To Primal.csv";
    std::string TBT;
    while (true) {
        std::cout << "Input string to be translated to english: ";
        TBT = window.WindowIn(pipeOpen);

        PrimalToEnglish(PTE, TBT, pipeOpen);
    }
}
