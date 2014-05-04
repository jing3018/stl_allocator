#include <iostream>
#include <map>
#include <vector>
#include <string>

using namespace std;

int main(){
    string s;
    string b = "this is a string";
    s.resize(100);
    s = b;
    cout << s << endl ;
    vector<int> a;
    a.push_back(10);
    a.push_back(20);
    map<string, int> d;
    d["what"] = 10;
    return 0;
}

