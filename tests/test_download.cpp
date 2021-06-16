#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>
#include <stdio.h>
#include <string.h>
#include <filesystem>
#include <stack>

#include "load.h"
#include "BlockHeader.h"
#include "Transaction.h"
#include "hashlib/sha256.h"
#include "Block.h"

#define CURL_STATICLIB 1
#include <curl/curl.h>
#pragma comment(lib, "libcurl_a.lib")

int hex2bin(unsigned char *dest, unsigned char *src, int len);

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

std::string GENESIS_BLOCK_HASH = "00000000839a8e6886ab5951d76f411475428afc90947ee320161bbf18eb6048";

void download_some();
int main(void)
{
     download_some();    return 0;
    char fn[128];
    //std::string next_block = "00000000edfa5bfffd21cc8ce76e46b79dc00196e61cdc62fd595316136f8a83"; // block 1024
    std::string next_block = "0000000000000000000d06cb8554f862f69825a7994dab6161ec0970e35f463e";  // recent as of writing
    while(true) {
        sprintf(fn, "bitcoindata/rawblock/%s.hex", next_block.c_str());
        std::string file_name(fn);

        std::stringstream ss;
        read_block_file(file_name, ss);
        std::cout << ss.str() << std::endl;
        Block block;
        block.unpack_hex(ss);

        std::stringstream output, out, outheader;
        block.pack_hex(output);
        auto outstr = output.str();
        hexdump(out, (unsigned char *) outstr.c_str(), outstr.length());
        std::cout << out.str() << std::endl;

        std::stringstream headerss;
        block.header.pack(headerss);
        std::string headerstr = headerss.str();
        hexdump(outheader, (unsigned char *) headerstr.c_str(), headerstr.length());
        std::cout << outheader.str() << std::endl;
        long lenh=outheader.str().length();
        assert(lenh == 320);
        char outh[320];
        hex2bin((unsigned char *)outh, (unsigned char *) outheader.str().c_str(), lenh);

        print_hash(outh, 80);

        if (std::string(next_block).compare(GENESIS_BLOCK_HASH) != 0)
        {
            // move next
            std::reverse((unsigned char *)block.header.prev_block, (unsigned char *)block.header.prev_block + 32);
            std::stringstream ssf;
            hexdump(ssf, block.header.prev_block, 32);
            file_name = ssf.str();
            std::cout << file_name << std::endl;
            next_block = file_name;
        }
        else
        {
            std::cout << "Reached genesis block. DONE." << std::endl;
            break;
        }
    }

    return 0;
}

void download_some()
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    char next_block[] ="00000000edfa5bfffd21cc8ce76e46b79dc00196e61cdc62fd595316136f8a83";//1024
    //char* next_block = "000000000000000000001ded7e91b5d29a08e8166044ef3fc06a0944e9877a95";
    //char next_block[] = "0000000000000000000d06cb8554f862f69825a7994dab6161ec0970e35f463e";
    char url[128];
    char fn[128];
    int count = 1024;

    if (curl)
    {
        std::stack<std::string> tblocks;
        while (count--)
        {
            tblocks.push(next_block);
            sprintf(fn, "bitcoindata/rawblock/%s.hex", next_block);
            std::string file_name(fn);

            if (!std::filesystem::exists(file_name))
            {
                sprintf(url, "https://blockchain.info/rawblock/%s?format=hex", next_block);
                std::cout << url << std::endl;

                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
                FILE *fp = fopen(fn, "w");
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                res = curl_easy_perform(curl);
                fclose(fp);
                curl_easy_reset(curl);
            }

            std::stringstream ss, ss2b, ss2;
            read_block_file(file_name, ss);
            std::string str1 = ss.str();
            int len1 = str1.length();
            Block block;
            block.unpack_hex(ss);

            block.pack_hex(ss2);

            std::string str2 = ss2.str();
            int len2 = str2.length();

            int mm = 0;
            while (str1[mm] == str2[mm])
            {
                mm++;
            }

            assert(len1 == len2);
            assert(!str1.compare(str2));

            for(int i=0;i<block.transaction_count;i++) 
            {   
                auto tx = block.transactions[i];
                // std::cout << tx.input_count << " ";
                // std::cout << tx.inputs[0].script_size << std::endl;
            }

            if (std::string(next_block).compare(GENESIS_BLOCK_HASH) != 0)
            {
                // move next
                std::reverse((unsigned char *)block.header.prev_block, (unsigned char *)block.header.prev_block + 32);
                std::stringstream ssf;
                hexdump(ssf, block.header.prev_block, 32);
                file_name = ssf.str();
                std::cout << file_name << std::endl;
                strcpy(next_block, file_name.c_str());
            }
            else
            {
                std::cout << "Reached genesis block. DONE." << std::endl;
                break;
            }
        }

        while(!tblocks.empty())
        {
            std::string cb = tblocks.top();
            tblocks.pop();
            std::cout << "\"" << cb << "\"," << std::endl;
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
}