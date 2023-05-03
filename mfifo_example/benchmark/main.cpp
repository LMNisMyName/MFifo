#include "widget.h"
#include "mfifo.hpp"

#include <QApplication>
#include <string>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <windows.h>

const int NUM = 100;

void writer(std::ofstream& output_fp, MFifo<std::string>& fifo)
{
    std::string elem;
    int count = 0;
    while(count < NUM) {
        bool success = fifo.read(elem);
        while(!success) {
            success = fifo.read(elem);
        }
        Sleep(100);
        output_fp << elem << std::endl;
        count++;
    }

    return;
}

void reader(std::ifstream& input_fp, MFifo<std::string>& fifo)
{
    std::string this_line;
    while(std::getline(input_fp,this_line)){
        bool success = fifo.write(this_line);
        while(!success) {
            success = fifo.write(this_line);
        }
        Sleep(100);
    }
    return;
}

void Simu_MT()
{
    std::ofstream output_fp("output_MT.txt");
    std::ifstream input_fp("test.txt");
    MFifo<std::string> fifo(nullptr, 4096);
    auto start_time = std::chrono::system_clock::now();
    std::thread t_reader(reader, std::ref(input_fp), std::ref(fifo));
    std::thread t_writer(writer, std::ref(output_fp), std::ref(fifo));
    t_reader.join();
    t_writer.join();
    auto end_time = std::chrono::system_clock::now();
    auto elapse_time = end_time - start_time;
    std::cout << "Simu_MT use " << std::chrono::duration_cast<std::chrono::milliseconds>(elapse_time).count() << " us"<< std::endl;
    output_fp.close();
    input_fp.close();
    return;
}


void Simu_ST()
{
    std::ifstream input_fp("test.txt");
    std::ofstream output_fp("output_ST.txt");
    auto start_time = std::chrono::system_clock::now();
    std::string this_line;
    while(std::getline(input_fp,this_line)){
        Sleep(200);
        output_fp << this_line << std::endl;
    }
    auto end_time = std::chrono::system_clock::now();
    auto elapse_time = end_time - start_time;
    std::cout << "Simu_ST use " << std::chrono::duration_cast<std::chrono::milliseconds>(elapse_time).count() << " us"<< std::endl;
    input_fp.close();
    output_fp.close();
    return;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    Simu_ST();
    Simu_MT();
    return a.exec();
}
