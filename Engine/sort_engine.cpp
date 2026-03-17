//#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
//using namespace std;

int main(int c, char** v) {
    std::ios::sync_with_stdio(0); std::cin.tie(0);
    std::vector<int> a;
    for(int i = 1; i < c; ++i) a.push_back(std::stoi(v[i]));
    
    int n = a.size();
    for(int i = 0; i < n - 1; ++i) {
        for(int j = 0; j < n - 1 - i; ++j) {
            if(a[j] > a[j+1]) std::swap(a[j], a[j+1]);
            std::cout << j;
            for(int x : a) std::cout << " " << x;
            std::cout << "\n";
        }
    }
    std::cout << "-1";
    for(int x : a) std::cout << " " << x;
    std::cout << "\n";
}
