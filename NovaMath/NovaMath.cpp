
#include <iostream>
#include <iomanip>
#include "vm.h"
int main(int argc,char* argv[])
{
    if (argc < 2) return 1;
    try {
        VM vm;
        auto start = std::chrono::high_resolution_clock::now();
        vm.Init(argv[1]);
        auto compileEnd = std::chrono::high_resolution_clock::now();
        vm.Run();
        // 🏁 計測終了
        auto end = std::chrono::high_resolution_clock::now();

        // かかった時間をマイクロ秒（us）に変換
        auto compileDur = std::chrono::duration_cast<std::chrono::microseconds>(compileEnd - start).count();
        auto calcDur = std::chrono::duration_cast<std::chrono::microseconds>(end - compileEnd).count();
        std::cout << "--- 計測結果 ---" << std::endl;
        std::cout << "計算結果: " << std::setprecision(16) << '\n';
        vm.PrintAnswers();
        std::cout << "処理時間: コンパイル" << compileDur << " us (マイクロ秒)" << std::endl;
        std::cout << "          計算　　　" << calcDur << " us (マイクロ秒)" << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
