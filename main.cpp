#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <chrono>
#include <vector>

using namespace std;

struct carRecord {
    char plate[12];
    char brand[24];
    char owner[32];
};

struct indexRecord {
    char key[12];
    streamoff offset;
};

bool textToBinary(const string& textPath, const string& binPath) {
    ifstream in(textPath);
    ofstream out(binPath, ios::binary);

    if (in.fail() || out.fail()) {
        cout << "Error opening files" << endl;
        return false;
    }

    carRecord record;
    string tempPlate, tempBrand, tempOwner;

    while (in >> tempPlate >> tempBrand >> tempOwner) {
        memset(&record, 0, sizeof(record));

        strncpy(
            record.plate,
            tempPlate.c_str(),
            sizeof(record.plate) - 1
        );

        strncpy(
            record.brand,
            tempBrand.c_str(),
            sizeof(record.brand) - 1
        );

        strncpy(
            record.owner,
            tempOwner.c_str(),
            sizeof(record.owner) - 1
        );

        out.write(
            reinterpret_cast<char*>(&record),
            sizeof(carRecord)
        );
    }

    in.close();
    out.close();

    return true;
}

void printBinaryFile(const string& binPath) {
    ifstream in(binPath, ios::binary);

    if (in.fail()) {
        cout << "Error opening file" << endl;
        return;
    }

    cout << "Output of a binary file" << endl;

    carRecord record;
    int count = 0;

    while (in.read(
        reinterpret_cast<char*>(&record),
        sizeof(carRecord)
    )) {
        count++;

        cout << count << ") "
             << record.plate << " | "
             << record.brand << " | "
             << record.owner << endl;
    }

    in.close();
}

bool linearSearch(
    const string& binPath,
    const string& searchKey,
    bool showResult = true
) {
    ifstream in(binPath, ios::binary);

    if (!in.is_open()) {
        cout << "Error opening file for search" << endl;
        return false;
    }

    carRecord record;
    bool found = false;

    auto startTime = chrono::high_resolution_clock::now();

    while (in.read(
        reinterpret_cast<char*>(&record),
        sizeof(carRecord)
    )) {
        if (strcmp(record.plate, searchKey.c_str()) == 0) {
            found = true;
            break;
        }
    }

    auto endTime = chrono::high_resolution_clock::now();

    auto duration =
        chrono::duration_cast<chrono::microseconds>(
            endTime - startTime
        ).count();

    in.close();

    if (showResult) {
        if (found) {
            cout << "--- Record Found ---" << endl;

            cout << record.plate << " | "
                 << record.brand << " | "
                 << record.owner << endl;
        } else {
            cout << "Record with plate '"
                 << searchKey
                 << "' not found." << endl;
        }

        cout << "Time to search: "
             << duration
             << " microseconds" << endl;
    }

    return found;
}

long long measureSearchTime(
    const string& binPath,
    const string& searchKey,
    int repetitions
) {
    auto startTime = chrono::high_resolution_clock::now();

    for (int i = 0; i < repetitions; i++) {
        ifstream in(binPath, ios::binary);

        if (!in.is_open()) {
            return -1;
        }

        carRecord record;

        while (in.read(
            reinterpret_cast<char*>(&record),
            sizeof(carRecord)
        )) {
            if (strcmp(
                record.plate,
                searchKey.c_str()
            ) == 0) {
                break;
            }
        }

        in.close();
    }

    auto endTime = chrono::high_resolution_clock::now();

    auto totalTime =
        chrono::duration_cast<chrono::microseconds>(
            endTime - startTime
        ).count();

    return totalTime / repetitions;
}

void convertAllFiles() {
    cout << "\nConverting files..." << endl;

    if (textToBinary("cars100.txt", "cars100.bin")) {
        cout << "cars100.txt -> cars100.bin OK" << endl;
    }

    if (textToBinary("cars1000.txt", "cars1000.bin")) {
        cout << "cars1000.txt -> cars1000.bin OK" << endl;
    }

    if (textToBinary("cars10000.txt", "cars10000.bin")) {
        cout << "cars10000.txt -> cars10000.bin OK" << endl;
    }
}

void runBenchmark() {
    const int repetitions = 1000;

    struct TestData {
        string binFile;
        string searchKey;
        int recordCount;
    };

    TestData tests[] = {
        {"cars100.bin", "A099AA76", 100},
        {"cars1000.bin", "A999AA87", 1000},
        {"cars10000.bin", "T999AA94", 10000}
    };

    cout << "\nLinear search benchmark" << endl;
    cout << "Repetitions: "
         << repetitions
         << endl;

    cout << "\n";
    cout << "Records\t\tTime (microseconds)" << endl;
    cout << "--------------------------------" << endl;

    for (const TestData& test : tests) {
        long long time = measureSearchTime(
            test.binFile,
            test.searchKey,
            repetitions
        );

        if (time == -1) {
            cout << test.recordCount
                 << "\t\tError" << endl;
        } else {
            cout << test.recordCount
                 << "\t\t"
                 << time
                 << endl;
        }
    }
}

vector<indexRecord> createIndexTable(const string& binPath) {
    vector<indexRecord> index;

    ifstream in(binPath, ios::binary);

    if (!in.is_open()) {
        cout << "Error opening file for index creation"
             << endl;
        return index;
    }

    carRecord record;

    while (true) {
        streamoff offset = in.tellg();

        if (!in.read(
            reinterpret_cast<char*>(&record),
            sizeof(carRecord)
        )) {
            break;
        }

        indexRecord item{};

        strncpy(
            item.key,
            record.plate,
            sizeof(item.key) - 1
        );

        item.offset = offset;

        index.push_back(item);
    }

    in.close();

    return index;
}

void outputIndexTable(const vector<indexRecord>& index) {
    if (index.empty()) {
        cout << "Index table is empty." << endl;
        return;
    }

    cout << "\nIndex table" << endl;
    cout << "--------------------------------" << endl;
    cout << "Number\tKey\t\tOffset" << endl;
    cout << "--------------------------------" << endl;

    for (size_t i = 0; i < index.size(); i++) {
        cout << i + 1
             << "\t"
             << index[i].key
             << "\t\t"
             << index[i].offset
             << endl;
    }
}

int fibonacciSearch(
    const vector<indexRecord>& index,
    const string& searchKey
) {
    int n = static_cast<int>(index.size());

    int fibM2 = 0;
    int fibM1 = 1;
    int fibM = fibM2 + fibM1;

    while (fibM < n) {
        fibM2 = fibM1;
        fibM1 = fibM;
        fibM = fibM2 + fibM1;
    }

    int offset = -1;

    while (fibM > 1) {
        int i = min(offset + fibM2, n - 1);

        int comparison = strcmp(
            index[i].key,
            searchKey.c_str()
        );

        if (comparison < 0) {
            fibM = fibM1;
            fibM1 = fibM2;
            fibM2 = fibM - fibM1;
            offset = i;
        } else if (comparison > 0) {
            fibM = fibM2;
            fibM1 = fibM1 - fibM2;
            fibM2 = fibM - fibM1;
        } else {
            return i;
        }
    }

    if (fibM1 && offset + 1 < n &&
        strcmp(
            index[offset + 1].key,
            searchKey.c_str()
        ) == 0) {
        return offset + 1;
    }

    return -1;
}

bool fibonacciSearchFile(
    const string& binPath,
    const string& searchKey
) {
    vector<indexRecord> index = createIndexTable(binPath);

    if (index.empty()) {
        return false;
    }

    auto startTime = chrono::high_resolution_clock::now();

    int indexPosition = fibonacciSearch(
        index,
        searchKey
    );

    auto endTime = chrono::high_resolution_clock::now();

    auto searchTime =
        chrono::duration_cast<chrono::microseconds>(
            endTime - startTime
        ).count();

    if (indexPosition == -1) {
        cout << "Record with plate '"
             << searchKey
             << "' not found." << endl;

        cout << "Fibonacci search time: "
             << searchTime
             << " microseconds" << endl;

        return false;
    }

    ifstream in(binPath, ios::binary);

    if (!in.is_open()) {
        cout << "Error opening file." << endl;
        return false;
    }

    in.seekg(index[indexPosition].offset);

    carRecord record;

    if (!in.read(
        reinterpret_cast<char*>(&record),
        sizeof(carRecord)
    )) {
        cout << "Error reading record." << endl;
        in.close();
        return false;
    }

    in.close();

    cout << "--- Record Found ---" << endl;

    cout << record.plate << " | "
         << record.brand << " | "
         << record.owner << endl;

    cout << "Offset: "
         << index[indexPosition].offset
         << endl;

    cout << "Fibonacci search time: "
         << searchTime
         << " microseconds" << endl;

    return true;
}


void runFibonacciBenchmark() {
    const int repetitions = 1000;

    struct TestData {
        string binFile;
        string searchKey;
        int recordCount;
    };

    TestData tests[] = {
        {"cars100.bin", "A099AA76", 100},
        {"cars1000.bin", "A999AA87", 1000},
        {"cars10000.bin", "T999AA94", 10000}
    };

    cout << "\nFibonacci search benchmark" << endl;
    cout << "Repetitions: "
         << repetitions
         << endl;

    cout << "\n";
    cout << "Records\t\tTime (microseconds)" << endl;
    cout << "--------------------------------" << endl;

    for (const TestData& test : tests) {
        vector<indexRecord> index =
            createIndexTable(test.binFile);

        if (index.empty()) {
            cout << test.recordCount
                 << "\t\tError" << endl;
            continue;
        }

        auto startTime =
            chrono::high_resolution_clock::now();

        for (int i = 0; i < repetitions; i++) {
            int position =
                fibonacciSearch(
                    index,
                    test.searchKey
                );

            if (position != -1) {
                ifstream in(
                    test.binFile,
                    ios::binary
                );

                if (in.is_open()) {
                    carRecord record;

                    in.seekg(
                        index[position].offset
                    );

                    in.read(
                        reinterpret_cast<char*>(&record),
                        sizeof(carRecord)
                    );

                    in.close();
                }
            }
        }

        auto endTime =
            chrono::high_resolution_clock::now();

        auto totalTime =
            chrono::duration_cast<
                chrono::microseconds
            >(endTime - startTime).count();

        cout << test.recordCount
             << "\t\t"
             << totalTime / repetitions
             << endl;
    }
}

int main() {
    char menu;

    while (true) {
        cout << "\n"
             << "1. Convert text files to binary\n"
             << "2. Output a binary file\n"
             << "3. Search for a key\n"
             << "4. Benchmark linear search\n"
             << "5. Output index table\n"
             << "6. Fibonacci search\n"
             << "7. Benchmark Fibonacci search\n"
             << "0. Exit\n";

        cout << "Enter your choice: ";
        cin >> menu;

        switch (menu) {
            case '1': {
                convertAllFiles();
                break;
            }

            case '2': {
                int fileSize;

                cout << "Choose file size "
                     << "(100 / 1000 / 10000): ";
                cin >> fileSize;

                string binFile;

                if (fileSize == 100) {
                    binFile = "cars100.bin";
                } else if (fileSize == 1000) {
                    binFile = "cars1000.bin";
                } else if (fileSize == 10000) {
                    binFile = "cars10000.bin";
                } else {
                    cout << "Incorrect file size." << endl;
                    break;
                }

                printBinaryFile(binFile);
                break;
            }

            case '3': {
                int fileSize;
                string key;

                cout << "Choose file size "
                     << "(100 / 1000 / 10000): ";
                cin >> fileSize;

                string binFile;

                if (fileSize == 100) {
                    binFile = "cars100.bin";
                } else if (fileSize == 1000) {
                    binFile = "cars1000.bin";
                } else if (fileSize == 10000) {
                    binFile = "cars10000.bin";
                } else {
                    cout << "Incorrect file size." << endl;
                    break;
                }

                cout << "Enter license plate to search: ";
                cin >> key;

                linearSearch(binFile, key);
                break;
            }

            case '4': {
                runBenchmark();
                break;
            }

            case '5': {
                int fileSize;

                cout << "Choose file size "
                     << "(100 / 1000 / 10000): ";
                cin >> fileSize;

                string binFile;

                if (fileSize == 100) {
                    binFile = "cars100.bin";
                } else if (fileSize == 1000) {
                    binFile = "cars1000.bin";
                } else if (fileSize == 10000) {
                    binFile = "cars10000.bin";
                } else {
                    cout << "Incorrect file size." << endl;
                    break;
                }

                vector<indexRecord> index =
                    createIndexTable(binFile);

                outputIndexTable(index);
                break;
            }

            case '6': {
                int fileSize;
                string key;

                cout << "Choose file size "
                     << "(100 / 1000 / 10000): ";
                cin >> fileSize;

                string binFile;

                if (fileSize == 100) {
                    binFile = "cars100.bin";
                } else if (fileSize == 1000) {
                    binFile = "cars1000.bin";
                } else if (fileSize == 10000) {
                    binFile = "cars10000.bin";
                } else {
                    cout << "Incorrect file size." << endl;
                    break;
                }

                cout << "Enter license plate to search: ";
                cin >> key;

                fibonacciSearchFile(binFile, key);
                break;
            }

            case '7': {
                runFibonacciBenchmark();
                break;
            }

            case '0': {
                cout << "End Program" << endl;
                return 0;
            }

            default:
                cout << "----Incorrect input!----" << endl;
        }
    }
}