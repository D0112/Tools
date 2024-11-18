#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace std::filesystem;
using json = nlohmann::json;

struct FileData {
    std::string filename;
    std::string start_time;
    std::string end_time;
};

struct Sample {
    std::string root_path;
    std::vector<FileData> files;
};

bool parseTxtToSamples(const std::string& txtFilePath, std::vector<Sample>& samples) {
    std::ifstream txtFile(txtFilePath);
    if (!txtFile.is_open()) {
        std::cerr << "Failed to open .txt file: " << txtFilePath << std::endl;
        return false;
    }

    std::string line;
    Sample currentSample;
    while (std::getline(txtFile, line)) {
        if (line.empty()) {
            // If we encounter an empty line, assume end of a sample block
            if (!currentSample.root_path.empty()) {
                samples.push_back(currentSample);
                currentSample = Sample(); // Reset for next sample
            }
        }
        else {
            std::istringstream iss(line);
            std::string key;

            if (line.find("root_path:") == 0) {
                iss.ignore(10); // Skip "root_path: "
                std::getline(iss, currentSample.root_path);
            }
            else if (line.find("file:") == 0) {
                FileData fileData;
                iss.ignore(6); // Skip "file: "
                iss >> fileData.filename;

                iss.ignore(12); // Skip "start_time: "
                iss >> fileData.start_time;

                iss.ignore(10); // Skip "end_time: "
                iss >> fileData.end_time;

                currentSample.files.push_back(fileData);
            }
        }
    }

    // Add the last sample if exists
    if (!currentSample.root_path.empty()) {
        samples.push_back(currentSample);
    }

    return true;
}

void convertSamplesToJson(const std::vector<Sample>& samples, const std::string& jsonFilePath) {
    json j;
    for (const auto& sample : samples) {
        json sampleJson;
        sampleJson["root_path"] = sample.root_path;

        for (const auto& file : sample.files) {
            json fileJson;
            fileJson["filename"] = file.filename;
            fileJson["start_time"] = file.start_time;
            fileJson["end_time"] = file.end_time;
            sampleJson["files"].push_back(fileJson);
        }

        j["samples"].push_back(sampleJson);
    }

    // Write JSON to file
    std::ofstream jsonFile(jsonFilePath);
    if (jsonFile.is_open()) {
        jsonFile << j.dump(4); // Pretty print with 4-space indentation
        jsonFile.close();
    }
    else {
        std::cerr << "Failed to open .json file: " << jsonFilePath << std::endl;
    }
}

bool parseJson(const std::string& filepath, std::vector<Sample>& samples) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return false;
    }

    json j;
    file >> j;

    for (const auto& sample_json : j["samples"]) {
        Sample sample;
        sample.root_path = sample_json["root_path"];

        for (const auto& file_json : sample_json["files"]) {
            FileData file_data;
            file_data.filename = file_json["filename"];
            file_data.start_time = file_json["start_time"];
            file_data.end_time = file_json["end_time"];
            sample.files.push_back(file_data);
        }

        samples.push_back(sample);
    }
    int n = samples.size();
    samples.erase(samples.begin() + n / 2, samples.end());
    return true;
}

// ʹָ··Ƶ·ƵŲüƵƵ
void trimVideo(const std::string& inputFile, const std::string& outputDir, const std::string& fileName,const std::string& startTime, const std::string& endTime, int clipNumber) {
    // ȡԭƵļǰĸַļ
    std::string outputFileName = fileName.substr(0, 4) + "-" + std::to_string(clipNumber) + ".mp4";

    string newDir = outputDir + "/" + std::to_string(clipNumber);

    // Ŀ¼Ƿڣ򴴽
    if (!filesystem::exists(newDir)) {
        filesystem::create_directories(newDir);
    }

    // ļ·
    std::string outputFilePath = newDir + "/" + outputFileName;

    // ʹ FFmpeg нƵü
    /*std::ostringstream command;
    command << "ffmpeg -i " << inputFile
        << " -ss " << startTime
        << " -to " << endTime
        << " -c copy " << outputFilePath;*/
    std::ostringstream command;
    command <<  "ffmpeg -ss " << startTime
        << " -to " << endTime
        << " -i " << inputFile
        << " -c copy " << outputFilePath;

    // ִ
    int result = std::system(command.str().c_str());
    if (result == 0) {
        std::cout << "Video trimmed successfully: " << outputFilePath << std::endl;
    }
    else {
        std::cerr << "Error trimming video." << std::endl;
    }
}

int main() {
    std::vector<Sample> samples;
    std::string txtFilePath = "../data2.txt";
    std::string filepath = "../samples.json";
    std::string outputDir = "../Videos2/";
    if (parseTxtToSamples(txtFilePath, samples)) {
        convertSamplesToJson(samples, filepath);
        std::cout << "Conversion completed successfully!" << std::endl;
    }

    int clipNumber = 1;
    if (parseJson(filepath, samples)) {
        for (const auto& sample : samples) {
            for (const auto& file : sample.files) {
                std::string inputFile = sample.root_path+ "\\" + file.filename;
                std::string startTime = file.start_time;
                std::string endTime = file.end_time;
                trimVideo(inputFile, outputDir, file.filename, startTime, endTime, clipNumber);
            }
            ++clipNumber;
        }
    }
    else {
        std::cerr << "Error parsing JSON file." << std::endl;
    }

    return 0;
}
 