#include <iostream>
#include <string> 
#include <fstream>
#include <vector>
#include <sstream>
#include <queue>
using namespace std;

string title; // The 1st row of file csv

//Get id field
string getIdBook(string& row) {
    string id = "";
    int i = 0; 

    while (i < row.length() && row[i] != ',') {
        id += row[i++];
    }

    return id;
}

struct minHeapNode {
    string element;
    int pos; //the index of file this element belong to
    minHeapNode(string element, int pos) {
        this->element = element;
        this->pos = pos;
    }
};

//Compare 2 ids of 2 elements
struct compare {
    bool operator() (minHeapNode& x, minHeapNode& y) {
        return getIdBook(x.element) > getIdBook(y.element);
    }
};

void mergeFile(int numFile) {
    vector<fstream> f(numFile + 1);

    //Priority_queue data structure defined by min heap
    priority_queue<minHeapNode, vector<minHeapNode>, compare> pq;

    //Open all files and push first element into Priority_queue
    for (int i = 1; i <= numFile; ++i) {
        stringstream ss;
        ss << "file_" << i << ".csv";
        f[i].open(ss.str(), ios::in | ios::binary);
        string row;
        if (!f[i].eof()) {
            getline(f[i], row);
            pq.push(minHeapNode(row, i));
        }    
    }

    fstream fout;
    fout.open("sorted_books_rating.csv", ios::out | ios::binary);
    fout << title << '\n';

    //Merge process
    while (pq.size()) {
        string row = pq.top().element;
        int pos = pq.top().pos;
        pq.pop();
        if (row == "") continue;
        fout << row << '\n';  
        if (!f[pos].eof()) {
            getline(f[pos], row);
            pq.push(minHeapNode(row, pos));
        }
    }

    fout.close();
    for (int i = 1; i <= numFile; ++i) {
        f[i].close();
    }
}

int partition(vector<string>& data, int l, int r) {
    int i = l - 1, j = r;
    string v = data[r]; 

    for (;;) {
        while (getIdBook(data[++i]) < getIdBook(v));
        while (getIdBook(v) < getIdBook(data[--j])) {
            if (j == l) break;
        }
        if (i >= j) break;
        swap(data[i], data[j]);
    }

    swap(data[i], data[r]);
    return i;
}

//Quick sort memory optimize
void quickSort(vector<string>& data, int l, int r) {
    while (l < r) {
        int p = partition(data, l, r);
        if (p - l < r - p) {
            quickSort(data, l, p - 1);
            l = p + 1;
        } else  {
            quickSort(data, p + 1, r);
            r = p - 1;
        }
    }
}

void sortAndWrite(vector<string>& data, int numFile) {
    //Sort data using quick sort algorithm
    quickSort(data, 0, data.size() - 1);

    //Write data into file
    fstream fout;
    stringstream ss;
    ss << "file_" << numFile << ".csv";
    fout.open(ss.str(), ios::out | ios::binary);
    for (string x : data) {
        fout << x << '\n';
    }
    fout.close();
}

int readFileInChunk(fstream& fin, int chunkSize) {
    vector<string> data;
    string row;
    getline(fin, title); //Get title line
    int numFile = 1, curMem = title.length() + 1;
    
    while (!fin.eof()) {
        getline(fin, row);
        if (curMem + row.length() + 1 <= chunkSize) {
            curMem += row.length() + 1;
            data.push_back(row);
        } else { // Memory > ChunkSize
            sortAndWrite(data, numFile);
            cout << "   + Created and sorted file " << numFile << '\n';
            numFile++;
            data.clear();
            data.push_back(row);
            curMem = row.length() + 1;
        }
    }

    // Pop empty line
    while (data.size() && data.back() == "") {
        data.pop_back();
    }
    sortAndWrite(data, numFile);
    cout << "   + Created and sorted file " << numFile << '\n';
    data.clear();
    return numFile;
}

int main() {
    cout << "- Reading file Books_rating.csv in chunks....\n";
    fstream fin;
    fin.open("Books_rating.csv", ios::in | ios::binary);
    int chunkSize = 200000000; // bytes
    int numFile = readFileInChunk(fin, chunkSize);
    fin.close();

    cout << "- Merging " << numFile << " sorted files....\n";
    mergeFile(numFile);
    cout << "   + File 'sorted_books_rating.csv' was created successfully !!!\n";
    return 0;
} 
