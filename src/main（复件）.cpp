#include "ros/ros.h"
#include "serial/serial.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>


serial::Serial ser;

double read_acc(std::string data, int start_bit)
{
    char dot = '.';
    char a_1 = data[start_bit];
    char a_2 = data[start_bit + 1];
    char a_3[4];
    for (int i = 0; i < 4; i++)
    {
        a_3[i] = data[i + start_bit + 2];
    }
    bool a_1_ = a_1 - '0';
    int a_2_ = a_2 - '0';
    int a_3_ = a_3[0] - '0';
    int a_4_ = a_3[1] - '0';
    int a_5_ = a_3[2] - '0';
    int a_6_ = a_3[3] - '0';
    int decimal = dot == '.' ? 1 : 0;
    double a;
    if (a_1_)
    {
        a = (a_2_ + a_3_ * pow(10, -1 * decimal) + a_4_ * pow(100, -1 * decimal) + a_5_ * pow(1000, -1 * decimal) + a_6_ * pow(10000, -1 * decimal));
    }
    else
    {
        a = -(a_2_ + a_3_ * pow(10, -1 * decimal) + a_4_ * pow(100, -1 * decimal) + a_5_ * pow(1000, -1 * decimal) + a_6_ * pow(10000, -1 * decimal));
    }

    // float a = (a_2_ + a_3_ * pow(10, -1 * decimal) + a_4_ * pow(100, -1 * decimal) + a_5_ * pow(1000, -1 * decimal) + a_6_ * pow(10000, -1 * decimal));

    return a;
}

double read_gyo(std::string data, int start_bit)
{
    char dot = '.';
    char g_1 = data[start_bit];
    char g_2[3];
    for (int i = 0; i < 3; i++)
    {
        g_2[i] = data[i + start_bit + 1];
    }
    char g_3[2];
    for (int i = 0; i < 2; i++)
    {
        g_3[i] = data[i + start_bit + 4];
    }
    bool g_1_ = g_1 - '0';
    int g_2_ = g_2[0] - '0';
    int g_3_ = g_2[1] - '0';
    int g_4_ = g_2[2] - '0';

    int g_5_ = g_3[0] - '0';
    int g_6_ = g_3[1] - '0';

    int decimal = dot == '.' ? 1 : 0;
    double g;
    if (g_1_)
    {
        g = (g_2_ * 100 + g_3_ * 10 + g_4_ + g_5_ * pow(10, -1 * decimal) + g_6_ * pow(100, -1 * decimal));
    }
    else
    {
        g = -(g_2_ * 100 + g_3_ * 10 + g_4_ + g_5_ * pow(10, -1 * decimal) + g_6_ * pow(100, -1 * decimal));
    }

    // float a = (a_2_ + a_3_ * pow(10, -1 * decimal) + a_4_ * pow(100, -1 * decimal) + a_5_ * pow(1000, -1 * decimal) + a_6_ * pow(10000, -1 * decimal));

    return g;

}


int main(int argc, char **argv)
{
    try
    {
        ser.setPort("/dev/ttyUSB0");
        ser.setBaudrate(115200);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
    }
    catch(serial::IOException& e)
    {
        std::cout << "unable to open port " << std::endl;
        return 0;
    }
    if(ser.isOpen())
    {
        std::cout << "serial port initialized " << std::endl;
    }
    else
    {
        return 0;
    }
    // initalize the size of data
    size_t data_size;
    // define the send data
    unsigned char send_buffer_data[6];
    send_buffer_data[0] = 0x77;
    send_buffer_data[1] = 0x04;
    send_buffer_data[2] = 0x00;
    send_buffer_data[3] = 0x59;
    send_buffer_data[4] = 0x5D; 
    // 接收回传报文
    std::string response;




    auto start = std::chrono::steady_clock::now();
    
    int k = 1;
    while(1)
    {
        std::cout << k << std::endl;
        ser.write(send_buffer_data,5);
        while(ser.available()!=48);
    
        if(data_size = ser.available())
        {
            // std::cout << data_size << std::endl;
            // 读取数据
            std::vector<unsigned char> tmpdata(data_size);
            std::size_t t = 0;
            // printf("%c\n", tmpdata[t]);
            ser.read(tmpdata.data(), data_size);

            // 转换数据并打印
            response.resize(data_size * 2);
            static constexpr char hex_digits[] = "0123456789ABCDEF";
            for (std::size_t i = 0; i < data_size; ++i)
            {
                response[i * 2] = hex_digits[tmpdata[i] >> 4];
                response[i * 2 + 1] = hex_digits[tmpdata[i] & 0xF];
            }
            // std::cout << response << std::endl;
        }
        k++;
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);

        double time = duration.count();
        std::cout << time << " " << read_gyo(response, 8) << " " << read_gyo(response,14) << " " << read_gyo(response, 20) << " " << read_acc(response, 26) << " " << read_acc(response, 32) << " " << read_acc(response, 38) << std::endl;
    }


    // ser.write(send_buffer_data,5);

    
    

    return 0;
}
