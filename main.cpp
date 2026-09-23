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
        cerr << "Error opening files" << endl;
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

int main() {
    char menu;
    while (true) {
        cout << ""
        << ""
        << "";
        cin >> menu;

        switch (menu) {
            case '1':
            case '0':
                cout << "End Program";
                return 0;
            default:
                cout << "----Incorrect input!----";
        }
    }
}