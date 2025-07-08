/*!
 * Copyright (c) 2025 Xinwu Yu
 *
 *
 * This program is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */
/*!
 * @file onptBWT.hpp
 * @brief Compute onptBWT.
 * @author Xinwu Yu
 * @date 2025-2-14
 */

#include <stdint.h>

#include <time.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sys/resource.h>
#include <sys/time.h>

#include "cmdline.h"
#include "OnlineRlbwt.hpp"
#include "DynRleForRlbwt.hpp"
#include "IOutils.hpp"

using namespace itmmti;
using namespace std;
using SizeT = uint64_t;

int main(int argc, char *argv[])
{

    cmdline::parser parser;
    parser.add<std::string>("input", 'i', "input file name", true);
    parser.add<std::string>("output", 'o', "output file name BWT", false);
    parser.add<bool>("check", 0, "check correctness", false, 0);
    parser.add("help", 0, "print help");

    parser.parse_check(argc, argv);
    const std::string in = parser.get<std::string>("input");
    const std::string out = parser.get<std::string>("output");
    const bool checkBool = parser.get<bool>("check");
    auto t1 = std::chrono::high_resolution_clock::now();

    using BTreeNodeT = BTreeNode<32>;
    using BtmNodeMT = BtmNodeM_StepCode<BTreeNodeT, 32>;
    using BtmMInfoT = BtmMInfo_BlockVec<BtmNodeMT, 512>;
    using BtmNodeST = BtmNodeS<BTreeNodeT, uint32_t, 8>;
    using BtmSInfoT = BtmSInfo_BlockVec<BtmNodeST, 1024>;
    using RynRleT = DynRleForRlbwt<WBitsBlockVec<1024>, Samples_Null, BtmMInfoT, BtmSInfoT>;
    OnlineRlbwt<RynRleT> rlbwt(1);

    std::vector<char> Text;
    uint64_t n = 0, ns = 0;

    load_fasta_reverse(in, Text, n, ns);
    auto start = std::chrono::steady_clock::now();
    uint64_t cur_ns = 0, cur_n = 0;
    // std::cout << cur_ns;
    for (unsigned char c : Text)
    {
        rlbwt.sptExtend(uint8_t(c));
        cur_n++;
        if (c == '\x00')
        {
            cur_ns++;
            // std::cout << cur_ns;
            if (cur_ns % 10000 == 0)
            {
                auto end = std::chrono::steady_clock::now();
                std::cout << "===================extend over=======================" << std::endl;
                std::cout << "cur_ns:" << cur_ns << "  cur_n:" << cur_n << std::endl;
                std::cout << "Elapsed time in seconds: "
                          << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                          << " milliseconds" << std::endl;

                // start = std::chrono::steady_clock::now();
                // break;
            }
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "===================extend over=======================" << std::endl;
    std::cout << "cur_ns:" << cur_ns << "  cur_n:" << cur_n << std::endl;
    std::cout << "Elapsed time in seconds: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " milliseconds" << std::endl;
    rlbwt.printStatistics(std::cout, true);

    std::ostringstream oss;
    rlbwt.writeBWT(oss);
    std::string onptBWT = oss.str();
    if (!(out.empty()))
    {
        std::ofstream out_file(out); // 文件路径
        if (out_file.is_open())
        {
            out_file << onptBWT; // 写入内容
            out_file.close();    // 关闭文件
            std::cout << "The BWT data has been written to a file: " << out << std::endl;
        }
        else
        {
            std::cerr << "Can not open file！" << std::endl;
        }
    }

    if (checkBool)
    { // check correctness
        std::string deCodeStr = rlbwt.decompressOnptBWT();
        std::vector<std::string> deCodeStrs1 = split(deCodeStr, '$');

        load_fasta(in, Text, n);
        std::string inputStr(Text.begin(), Text.end());
        std::vector<std::string> deCodeStrs2 = split(inputStr, '\x00');
        if (check(deCodeStrs1, deCodeStrs2))
        {
            std::cout << "RLBWT decompressed correctly. " << std::endl;
        }
        else
        {
            std::cout << "RLBWT inversion failed." << std::endl;
        }
    }
}