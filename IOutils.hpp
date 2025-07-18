#ifndef INCLUDE_GUARD_IOUtils
#define INCLUDE_GUARD_IOUtils

#include <fstream>
#include <cstring>
#include <vector>
#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>

void load_fasta_reverse(std::string filename, std::vector<char> &Text, uint64_t &sum, uint64_t &ns)
{
    // open the input stream
    std::ifstream input(filename);
    if (!input.good())
    {
        std::cerr << "Error opening " << filename << ". exiting..." << std::endl;
        exit(-1);
    }
    input.seekg(0, std::ios::end);
    Text.resize(input.tellg());
    input.seekg(0, std::ios::beg);

    std::string line, DNA_sequence;
    sum = 0, ns = 0;
    while (std::getline(input, line))
    {
        // skip empty lines
        if (line.empty())
        {
            continue;
        }
        // header of a new sequence
        if (line[0] == '>')
        {
            ns++; // increase sequence count
            size_t seqlen = DNA_sequence.size();
            // insert previous DNA sequence
            if (seqlen > 0)
            {
                // 1. reverse DNA_sequence
                std::reverse(DNA_sequence.begin(), DNA_sequence.end());

                // 2. write Text
                memcpy(&Text[sum], &DNA_sequence[0], seqlen);
                // increase current text size
                sum += seqlen;
                // add separator
                Text[sum++] = 0;
            }
            // the current DNA sequence
            DNA_sequence.clear();
        }
        else
        {
            DNA_sequence += line;
        }
    }
    std::reverse(DNA_sequence.begin(), DNA_sequence.end());
    memcpy(&Text[sum], &DNA_sequence[0], DNA_sequence.size());
    sum += DNA_sequence.size();
    Text[sum++] = 0;
    Text.resize(sum);
    Text.shrink_to_fit();
    // close stream
    input.close();
}

bool load_fasta(std::string filename, std::vector<char> &Text, uint64_t &sum)
{
    Text.clear(); // Clear the original data
    sum = 0;      // Reset the length counter
    // open the input stream
    std::ifstream input(filename);
    if (!input.good())
    {
        std::cerr << "Error opening " << filename << ". exiting..." << std::endl;
        exit(-1);
    }
    input.seekg(0, std::ios::end);
    Text.resize(input.tellg());
    input.seekg(0, std::ios::beg);

    std::string line, DNA_sequence;
    while (std::getline(input, line))
    {
        // skip empty lines
        if (line.empty())
        {
            continue;
        }
        // header of a new sequence
        if (line[0] == '>')
        {
            size_t seqlen = DNA_sequence.size();
            // insert previous DNA sequence
            if (seqlen > 0)
            {
                // write Text
                memcpy(&Text[sum], &DNA_sequence[0], seqlen);
                // increase current text size
                sum += seqlen;
                // add separator
                Text[sum++] = '\x00';
            }
            // the current DNA sequence
            DNA_sequence.clear();
        }
        else
        {
            DNA_sequence += line;
        }
    }
    // std::reverse(DNA_sequence.begin(), DNA_sequence.end());
    memcpy(&Text[sum], &DNA_sequence[0], DNA_sequence.size());
    sum += DNA_sequence.size();
    Text[sum++] = '\x00';
    Text.resize(sum);
    Text.shrink_to_fit();
    // close stream
    input.close();
    return true;
}

void writeTextToFile(const std::string &filename, const std::vector<char> &Text)
{
    std::ofstream outfile(filename, std::ios::binary);

    if (!outfile)
    {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    if (!Text.empty())
    {
        outfile.write(reinterpret_cast<const char *>(Text.data()), Text.size());

        if (!outfile)
        {
            std::cerr << "Error: Failed to write data to " << filename << "." << std::endl;
        }
    }
    else
    {
        std::cerr << "Warning: Text vector is empty. Nothing was written to " << filename << "." << std::endl;
    }

    outfile.close();
}

void printHex(const std::string &s)
{
    for (char c : s)
    {
        printf("%02X ", static_cast<unsigned char>(c));
    }
    std::cout << std::endl;
}

std::vector<std::string> split(std::string s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;

    for (char c : s)
    {
        if (c == delimiter)
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        else
        {
            token += c;
        }
    }

    if (!token.empty())
    {
        tokens.push_back(token);
    }

    std::sort(tokens.begin(), tokens.end());

    return tokens;
}

void printComparison(const std::vector<std::string> &v1, const std::vector<std::string> &v2)
{
    std::cout << "==== Vector Comparison ====\n";

    // 打印向量大小
    std::cout << "v1 size: " << v1.size() << "\n";
    std::cout << "v2 size: " << v2.size() << "\n";

    // 打印表格标题
    std::cout << std::left;
    std::cout << std::setw(10) << "Index"
              << std::setw(20) << "v1"
              << std::setw(20) << "v2"
              << "Status\n";
    std::cout << std::string(60, '-') << "\n";

    // 比较每个元素
    size_t maxSize = std::max(v1.size(), v2.size());
    bool allEqual = true;

    for (size_t i = 0; i < maxSize; ++i)
    {
        bool hasV1 = i < v1.size();
        bool hasV2 = i < v2.size();

        std::cout << std::setw(10) << i;

        // 打印 v1 元素
        if (hasV1)
        {
            std::cout << std::setw(20) << ("\"" + v1[i] + "\"");
        }
        else
        {
            std::cout << std::setw(20) << "<missing>";
        }

        // 打印 v2 元素
        if (hasV2)
        {
            std::cout << std::setw(20) << ("\"" + v2[i] + "\"");
        }
        else
        {
            std::cout << std::setw(20) << "<missing>";
        }

        // 比较状态
        if (!hasV1 || !hasV2)
        {
            std::cout << "SIZE MISMATCH";
            allEqual = false;
        }
        else if (v1[i] != v2[i])
        {
            std::cout << "DIFFERENT";
            allEqual = false;

            // 打印详细差异
            std::cout << "\n  Details: ";
            if (v1[i].size() != v2[i].size())
            {
                std::cout << "Lengths differ (" << v1[i].size()
                          << " vs " << v2[i].size() << ")";
            }
            else
            {
                for (size_t j = 0; j < v1[i].size(); ++j)
                {
                    if (v1[i][j] != v2[i][j])
                    {
                        std::cout << "Char at pos " << j << ": '"
                                  << v1[i][j] << "' (0x" << std::hex << static_cast<int>(v1[i][j])
                                  << ") vs '" << v2[i][j] << "' (0x"
                                  << static_cast<int>(v2[i][j]) << std::dec << ")";
                        break;
                    }
                }
            }
        }
        else
        {
            std::cout << "equal";
        }

        std::cout << "\n";
    }

    std::cout << "\nOverall: " << (allEqual ? "VECTORS ARE EQUAL" : "VECTORS DIFFER")
              << "\n=================================\n\n";
}

// 修改后的 check 函数
bool check(const std::vector<std::string> &v1, const std::vector<std::string> &v2)
{
    bool result = (v1.size() == v2.size()) && std::equal(v1.begin(), v1.end(), v2.begin());

    if (!result)
    {
        printComparison(v1, v2);
    }

    return result;
}

#endif