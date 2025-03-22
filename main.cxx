#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <bitset>
#include <cstdio>
#include <memory>
#include <cstdlib>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#define PATH_MAX 4096
#else
#include <unistd.h>
#include <limits.h>
#endif

#define forloop(len) for(int i = 0; i< len; i++)

std::string GetFilePath() {
    wchar_t result[PATH_MAX];

#ifdef _WIN32
    GetModuleFileName(NULL, result, PATH_MAX);
    std::wstring ws(result);
    std::string path(ws.begin(), ws.end());
    size_t directory = path.find_last_of("\\/");
    return path.substr(0, directory);

#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        std::string path(result, count);
        return path;
    }
    std::cerr << "Error: directory not found" << std::endl;
    return "";
#endif
}

bool PrintResult(std::vector<std::string>& matches) {
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

std::string PingFiles() {
    try {
        std::string directory = GetFilePath();
        std::ifstream EngPri("Skyhawk English To Primal.csv");
        std::ifstream PriEng("Skyhawk Primal To English.csv");
        if (!EngPri) {
            std::cerr << "Failed to open file: Skyhawk English To Primal.csv" << std::endl;
            throw 56;
        }
        if (!PriEng) {
            std::cerr << "Failed to open file: Skyhawk Primal To English.csv" << std::endl;
            throw 56;
        }
        return "Done";
    }
    catch (int Error) {
        printf("Unable to locate csv files\n");
        try {
            std::cout << "Running python script ... ";
            std::string DirectoryPath = GetFilePath();
            if (DirectoryPath.empty()) {
                throw std::runtime_error("Unable to locate executable path");
            }

            std::string pythonPath = DirectoryPath + "/XlsxToCsv.py";
            std::string fullPythonPath = "python3 " + pythonPath;

            std::unique_ptr<FILE, int(*)(FILE*)> pipe(_popen(fullPythonPath.c_str(), "r"), _pclose);
            if (!pipe) {
                std::cerr << "Error opening pipe" << std::endl;
                throw std::runtime_error("Error opening pipe");
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
        catch (const std::exception& error) {
            std::cout << "Unable to locate csv files and unable to run python script: " << error.what() << std::endl;
            exit(EXIT_FAILURE);
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

std::vector<std::string> PrimalToEnglish(const std::string& filename, const std::string& TBT) {
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
            PrintResult(matches);
        }
    }
    if (!perfectMatch) {
        for (const auto& row : data) {
            if (row[Primal_colom].find(TBT) < row[Primal_colom].length()) {
                matches.push_back(row[English_colom]);
            }
        }
        std::cout << matches.size();
        PrintResult(matches);
    }
    return matches;
}
std::vector<std::string> EnglishToPrimal(const std::string& filename, const std::string& TBT) {
    const int GrammaticalType_colom = 1;
    const int Primal_colom = 2;
    const int TricksterTransscription_colom = 4;
    const int SkyhawkTransscription_colom = 5;
    const int English_colom = 0;
    auto data = readCSV(filename);
    std::vector<std::string> matches;

    bool perfectMatch = false;

    for (const auto& row : data) {
        if (row[Primal_colom] == TBT) {
            perfectMatch = true;
            matches.push_back(row[English_colom]);
            PrintResult(matches);
        }
    }
    if (!perfectMatch) {
        for (const auto& row : data) {
            if (row[Primal_colom].find(TBT) < row[Primal_colom].length()) {
                matches.push_back(row[English_colom]);
            }
        }
        PrintResult(matches);
    }
    return matches;
}

int main() {
    std::string re = PingFiles();
    if (re.find("Error") != std::string::npos) {
        std::cerr << "Error in running python script" << std::endl;
        std::cout << re << std::endl;
        exit(EXIT_FAILURE);
    }

    const std::string PTE = "Skyhawk Primal To English.csv";
    const std::string ETP = "Skyhawk English To Primal.csv";

    std::string TBT;
    std::cout << "Translate to English or Primal? (E/P) ";
    std::cin >> TBT;
    if (TBT != "E") {
        while (true) {
            std::cout << "Input string to be translated to english: ";
            std::cin >> TBT;

            PrimalToEnglish(PTE, TBT);
        }
    }
    else {
        while (true) {
            std::cout << "Input string to be translated to primal: ";
            std::cin >> TBT;

            EnglishToPrimal(PTE, TBT);
        }
    }
}
