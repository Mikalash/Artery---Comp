#include <bits/stdc++.h>
#include "systemDefinations.h"

#pragma pack(push, 1)
typedef struct {
	SystemParams_st SystemParams;//124 bytes
	uint8_t SystemParams_crc;//4 bytes            //CRC of SystemParams
} SystemParams_shell;
#pragma pack(pop)

#define SP_type_size sizeof(SystemParams_st)
#define SP_shell_size sizeof(SystemParams_shell)

uint8_t crc8(const uint8_t* data, uint32_t data_size);

using namespace std;

uint8_t my_rand()
{
    int kek = rand() % 100;
    if (kek < 30)
        return 192;
    if (kek < 60)
        return 219;
    uint8_t ret_val = (uint8_t) rand();
    return ret_val;
}

float my_f_rand()
{
    float kek = (float) rand() / 100000;
    //if (rand() % 2 == 0)
        //kek *= -1;
    return kek;
}

int main()
{
    cout << SP_type_size;
    ofstream out;
    for (int i = 0; i < 10; i++)
    {
        SystemParams_shell SP;
        for (int j = 0; j < SP_type_size; j++)
            ((uint8_t*)(&SP.SystemParams))[j] = my_rand();
        SP.SystemParams.posSensor = (posSensorType)(rand() % 5 + 1);
        SP.SystemParams.phase_inverse = i;

        SP.SystemParams_crc = crc8((uint8_t*)(&SP.SystemParams), SP_type_size);
        //by_byte------------------------------------------------------------------------
        out.open("tests/" + to_string(i + 1) + "/" + "by_byte.txt");
        for(int j = 0; j < SP_shell_size; j++)
            out << ((char*)(&SP))[j];
        out.close();
        //slip_uart----------------------------------------------------------------------
        out.open("tests/" + to_string(i + 1) + "/" + "slip_uart.txt");
        out << (char) 255;
        for(int j = 0; j < SP_shell_size; j++)
        {
            switch(((uint8_t*)(&SP))[j])
            {
			case 192:
				out << (char) 219;
				out << (char) 220;
				break;
			case 219:
				out << (char) 219;
				out << (char) 221;
				break;
			default:
				out << ((char*)(&SP))[j];
            }
        }
        out << (char) 192;
        out.close();
    }
    return 0;
}

uint8_t crc8(const uint8_t* data, uint32_t data_size)
{
	uint8_t crc = 0xFF;
	uint32_t i;
	uint32_t j = 0;

	if (data == NULL)
		return 0;

	while (data_size--)
	{
		crc ^= *data++;
		for (i = 0; i < 8; i++)
			crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
	}
	return crc;
}
