#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <bitset>
#include <cstdio>
#include <memory>
using namespace std;

#define forloop(len) for(int i = 0; i< len; i++)
bool PrintResult(std::vector<std::string>& matches){
    if (matches.size() == 1){
        std::cout << "The translation is: " << matches[0] << std::endl;
        return true;
    }
    else if(matches.size() > 1){
        std::cout << "Possible translations: ";
        forloop(matches.size()){
            std::cout << "\n" << matches[i];
        }
        std::cout << std::endl;
        return true;
    }
    else{
        std::cout << "Found no translation" << std::endl;
        return true;
    }
}

std::string PingFiles(){
    try{
        std::ifstream EngPri("Skyhawk English To Primal.csv");
        std::ifstream PriEng("Skyhawk Primal To English.csv");
        if (!EngPri){
            std::cout << "Failed to open file: " << "Skyhawk English To Primal.csv" << std::endl;
            throw("File not found");
        }
        if (!PriEng){
            std::cout << "Failed to open file: " << "Skyhawk Primal To English.csv" << std::endl;
            throw("File not found");
        }
        return "Done";
    }
    catch(std::string Error){
        try{
            std::cout << "Running python script ... ";
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("python3 XlsxToCsv.py", "r"), pclose);
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
        catch (std::string error){
            std::cout << "Unable to locate csv files and unable to run python script" << std::endl;
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
            } else if (c == '"') {
                inQuotes = !inQuotes;
            } else {
                field += c;
            }
        }
        fields.push_back(field);
        data.push_back(fields);
    }

    file.close();
    return data;
}

std::vector<std::string> PrimalToEnglish(const std::string& filename, const std::string& TBT){
    const int GrammaticalType_colom = 1;
    const int Primal_colom = 2;
    const int TricksterTransscription_colom = 3;
    const int SkyhawkTransscription_colom = 4;
    const int English_colom = 5;
    auto data = readCSV(filename);
    std::vector<std::string> matches;

    bool perfectMatch = false;

    for(const auto& row : data){
        if (row[Primal_colom] == TBT){
            perfectMatch = true;
            matches.push_back(row[English_colom]);
            PrintResult(matches);
        }
    }
    if(!perfectMatch){
        for(const auto& row : data){
            if (row[Primal_colom].find(TBT) < row[Primal_colom].length()){
                matches.push_back(row[English_colom]);
            }
        }
        std::cout << matches.size();
        PrintResult(matches);
    }
    return matches;
}

int main(){
    std::string re = PingFiles();
    if (!re.find("Error")){
        std::cerr << "Error in running python script";
        std::cout << re << std::endl;
        exit(EXIT_FAILURE);
    }

    const std::string PTE = "Skyhawk English To Primal.csv";
    std::string TBT;
    while (true){
        std::cout << "Input string to be translated to english: ";
        std::cin >> TBT;
        
        PrimalToEnglish(PTE, TBT);
    }
}
