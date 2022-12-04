#include <iostream>
#include <string> 
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;
string title;

void Write(vector<string>& data, int numFile) {
    fstream fout;
    stringstream ss;
    ss << "file_" << numFile << ".csv";
    fout.open(ss.str(), ios::out | ios::binary);
    for (string x : data) {
        fout << x << '\n';
    }
    fout.close();
}


void splitFile(fstream& fin, int memoryLimit) {
    vector<string> data;
    string row;
    getline(fin, title);
    int numFile = 1, curMem = title.length() + 1;
    while (!fin.eof()) {
        getline(fin, row);
        if (curMem + row.length() + 1 <= memoryLimit) {
            curMem += row.length() + 1;
            data.push_back(row);
        } else {
            cout << "-> File " << numFile << " was created !\n";
            Write(data, numFile);
            numFile++;
            data.clear();
            data.push_back(row);
            curMem = row.length() + 1;
        }
    }
    data.pop_back(); 
    data.clear();

}

int main() {
    cout << "Spliting file Books_rating.csv....\n";
    fstream fin;
    fin.open("Books_rating.csv", ios::in | ios::binary);
    int memoryLimit = 3e8;
    splitFile(fin, memoryLimit);
    fin.close();
    return 0;
}
 