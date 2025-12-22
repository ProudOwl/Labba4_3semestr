#include <iostream>
#include "DynamicArray.h"

using namespace std;

void fill_row(DynamicArray<int>& arr, const int* vals, int count) {
    for(int i=0; i<count; ++i) arr.push_back(vals[i]);
}

bool isSafe(DynamicArray<int>& avail, DynamicArray<DynamicArray<int>>& maxm, DynamicArray<DynamicArray<int>>& allot) {
    int P = allot.get_size();
    int R = avail.get_size();

    DynamicArray<DynamicArray<int>> need(P);
    for (int i = 0; i < P; i++) {
        DynamicArray<int> row;
        for (int j = 0; j < R; j++) {
            row.push_back(maxm[i][j] - allot[i][j]);
        }
        need.push_back(move(row));
    }

    DynamicArray<bool> finish(P);
    for(int i=0; i<P; ++i) finish.push_back(false);

    DynamicArray<int> safeSeq(0);
    DynamicArray<int> work(R);
    
    for (int i = 0; i < R; i++) work.push_back(avail[i]);

    int count = 0;
    while (count < P) {
        bool found = false;
        for (int p = 0; p < P; p++) {
            if (!finish[p]) {
                int j;
                for (j = 0; j < R; j++)
                    if (need[p][j] > work[j])
                        break;

                if (j == R) {
                    for (int k = 0; k < R; k++)
                        work[k] += allot[p][k];
                    
                    safeSeq.push_back(p);
                    finish[p] = true;
                    found = true;
                    count++;
                }
            }
        }
        if (!found) {
            cout << "System is NOT in safe state (Deadlock)." << endl;
            return false;
        }
    }

    cout << "System is in SAFE state.\nSequence: ";
    for (int i = 0; i < safeSeq.get_size(); i++)
        cout << safeSeq[i] << (i == safeSeq.get_size() - 1 ? "" : " -> ");
    cout << endl;
    return true;
}

int main() {
    cout << "ЗАДАНИЕ 3: Алгоритм Банкира" << endl;

    DynamicArray<int> avail;
    int avVals[] = {3, 3, 2};
    fill_row(avail, avVals, 3);

    DynamicArray<DynamicArray<int>> maxm;
    int m0[] = {7, 5, 3}; DynamicArray<int> r0; fill_row(r0, m0, 3); maxm.push_back(move(r0));
    int m1[] = {3, 2, 2}; DynamicArray<int> r1; fill_row(r1, m1, 3); maxm.push_back(move(r1));
    int m2[] = {9, 0, 2}; DynamicArray<int> r2; fill_row(r2, m2, 3); maxm.push_back(move(r2));
    int m3[] = {2, 2, 2}; DynamicArray<int> r3; fill_row(r3, m3, 3); maxm.push_back(move(r3));
    int m4[] = {4, 3, 3}; DynamicArray<int> r4; fill_row(r4, m4, 3); maxm.push_back(move(r4));

    DynamicArray<DynamicArray<int>> allot;
    int a0[] = {0, 1, 0}; DynamicArray<int> ar0; fill_row(ar0, a0, 3); allot.push_back(move(ar0));
    int a1[] = {2, 0, 0}; DynamicArray<int> ar1; fill_row(ar1, a1, 3); allot.push_back(move(ar1));
    int a2[] = {3, 0, 2}; DynamicArray<int> ar2; fill_row(ar2, a2, 3); allot.push_back(move(ar2));
    int a3[] = {2, 1, 1}; DynamicArray<int> ar3; fill_row(ar3, a3, 3); allot.push_back(move(ar3));
    int a4[] = {0, 0, 2}; DynamicArray<int> ar4; fill_row(ar4, a4, 3); allot.push_back(move(ar4));

    isSafe(avail, maxm, allot);

    return 0;
}
