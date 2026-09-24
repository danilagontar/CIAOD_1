#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

struct carRecord {
    char plate[12];
    char brand[24];
    char owner[32];
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
        strncpy(record.plate, tempPlate.c_str(), sizeof(record.plate) - 1);
        strncpy(record.brand, tempBrand.c_str(), sizeof(record.brand) - 1);
        strncpy(record.owner, tempOwner.c_str(), sizeof(record.owner) - 1);
        out.write(reinterpret_cast<char*>(&record), sizeof(carRecord));
    }
    in.close();
    out.close();

    return true;
}

void printBinaryFile(const string& binPath) {
    ifstream in(binPath, ios::binary);
    if (in.fail()) {
        cout << "Error opening files" << endl;
        return;

    }
    cout << "Output of a binary file" << endl;
    carRecord record;
    int count = 0;

    while (in.read(reinterpret_cast<char*>(&record), sizeof(carRecord))) {
        count++;
        cout << count << ") "
             << record.plate << " | "
             << record.brand << " | "
             << record.owner << endl;
    }

    in.close();
}




int main() {
    char menu;
    while (true) {
        cout << "\n1. Convert the text to binary\n"
        << "2. Output of a binary file\n"
        << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> menu;

        switch (menu) {
            case '1': {
                string textFile = "cars100.txt";
                string binFile = "cars100.bin";
                if (textToBinary(textFile, binFile)) {
                    cout << "Binary File Successfully Converted" << endl;
                }
                break;
            }
            case '2': {
                string binFile = "cars100.bin";
                printBinaryFile(binFile);
                break;
            }
            case '0': {
                cout << "End Program";
                return 0;
            }
            default:
                cout << "----Incorrect input!----";
        }
    }
}