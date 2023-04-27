#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <cstring>
#include "serial/serial.h"
#include <math.h>
#include <chrono>

#include <fstream>

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

using namespace std;


int main()
{

    ofstream outfile("/home/jadis/git/catkin_serial/src/serial_test/data/record.csv");
    outfile << "time(ms),angle_x(deg),angle_y(deg),angle_z(deg),acc_x(g),acc_y(g),acc_z(g),motor_pos\n";
    auto start = chrono::steady_clock::now();
    try
    {
        // 设置串口参数
        serial::Serial imu_ser("/dev/ttyUSB0", 115200, serial::Timeout::simpleTimeout(100));
        serial::Serial motor_ser("/dev/ttyUSB1", 9600, serial::Timeout::simpleTimeout(100));
        
        
        // 待发送的报文
        uint8_t imu_send_data[] = {0x77, 0x04, 0x00, 0x59, 0x5D};
        uint8_t motor_send_data[] = {0x01, 0x03, 0x00, 0x57, 0x00, 0x0C, 0xF4, 0x1F};
        int imu_send_size = sizeof(imu_send_data) / sizeof(uint8_t);
        int motor_send_size = sizeof(motor_send_data) / sizeof(uint8_t);


        // 接收回传报文
        std::string imu_response;
        std::string motor_response;


        int k = 1;
        // 循环发送报文
        while (k < 1000)
        {
            cout << k << endl;
            imu_ser.write(imu_send_data, imu_send_size);
            motor_ser.write(motor_send_data, motor_send_size);


            // 接收回传报文
            vector<uint8_t> imu_recv_data;
            vector<uint8_t> motor_recv_data;


            imu_ser.read(imu_recv_data, 48);
            motor_ser.read(motor_recv_data,29);


            if (imu_recv_data.size() != 48)
            {
                // cout << "Error: Failed to read 5 bytes from serial port." << endl;
                continue;
            }
            if (motor_recv_data.size() != 29)
            {
                // cout << "Error: Failed to read 5 bytes from serial port." << endl;
                continue;
            }




            long motor_pos;
            vector<uint8_t> motor_pos_data = {motor_recv_data[6],motor_recv_data[5],motor_recv_data[4],motor_recv_data[3]};
            memcpy(&motor_pos, &motor_pos_data[0], sizeof(motor_pos));
            if (motor_pos > 1300000)
            {
                motor_pos = -1;
            }





            
            imu_response.resize(48*2);
            // motor_response.resize(29*2);


            static constexpr char hex_digits[] = "0123456789ABCDEF";

            for (std::size_t i = 0; i < 48; ++i)
            {
                imu_response[i * 2] = hex_digits[imu_recv_data[i] >> 4];
                imu_response[i * 2 + 1] = hex_digits[imu_recv_data[i] & 0xF];
            }
            /* for (std::size_t j = 0; j < 29; ++j)
            {
                motor_response[j * 2] = hex_digits[motor_recv_data[j] >> 4];
                motor_response[j * 2 + 1] = hex_digits[motor_recv_data[j] & 0xF];
            } */

            auto now = chrono::steady_clock::now();
            auto duration = chrono::duration_cast<std::chrono::milliseconds>(now-start);
            double time = duration.count();

            // cout << imu_response << endl;
            // cout << motor_pos << endl;
            // cout << time << " " << read_gyo(imu_response, 8) << " " << read_gyo(imu_response,14) << " " << read_gyo(imu_response, 20) << " " << read_acc(imu_response, 26) << " " << read_acc(imu_response, 32) << " " << read_acc(imu_response, 38) << endl;
            outfile << time << "," << read_gyo(imu_response, 8) << "," << read_gyo(imu_response,14) << "," << read_gyo(imu_response, 20) << "," << read_acc(imu_response, 26) << "," << read_acc(imu_response, 32) << "," << read_acc(imu_response, 38) << "," << motor_pos << "\n";



            // 输出接收到的数据
            /* cout << "Received: ";
            for (int i = 0; i < imu_recv_data.size(); i++)
            {
                printf("%02X ", imu_recv_data[i]);
            }
            cout << endl; */
            k++;
            imu_ser.flushInput();
            imu_ser.flushOutput();
            // 等待一段时间后再次发送报文
            usleep(10000);

        }
    }
    catch (exception& e)
    {
        cout << "Error: " << e.what() << endl;
    }
    outfile.close();
    return 0;
}
