#include "Serializable.h"

#include <iostream>
#include <fstream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        alloc_data(MESSAGE_SIZE);

        memset(_data, 0, MESSAGE_SIZE);
        
        char* it = _data;
        
        memcpy(it, name, MAX_NAME);
        it += MAX_NAME * sizeof(char);

        memcpy(it, &x, sizeof(int16_t));
        it += sizeof(int16_t);

        memcpy(it, &y, sizeof(int16_t));
        it += sizeof(int16_t);

        //para testear los valores iniciales
        std::cout << "to_bin(): " << name << " " << x << " " << y << '\n';
    }

    int from_bin(char * data)
    {
        alloc_data(MESSAGE_SIZE);

        memcpy(static_cast<void *>(_data), data, MESSAGE_SIZE);

        char* it = _data;

        //el único caso de error de la función que se nos ocurre es que se
        //sobrepase el tamaño de data, de ahi que usemos un try/catch para lanzar
        //el return -1

        try
        {
            memcpy(name, it, MAX_NAME);
            it += MAX_NAME * sizeof(char);

            memcpy(&x, it, sizeof(int16_t));
            it += sizeof(int16_t);

            memcpy(&y, it, sizeof(int16_t));
            it += sizeof(int16_t);
        }
        catch(const std::exception& e)
        {
            std::cerr << "from_bin() fail: " << e.what() << '\n';
            return -1;
        }

        //para testear los valores obtenidos
        std::cout << "from_bin(): " << name << " " << x << " " << y << '\n';

        return 0;
    }


public:
    static const size_t MAX_NAME = 80;
    char name[MAX_NAME];

    int16_t x;
    int16_t y;

    static const size_t MESSAGE_SIZE = sizeof(char) * MAX_NAME +  2 * sizeof(uint8_t);
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    one_w.to_bin();

    std::fstream file;

    //escritura (y, de no existir, previa creación) de archivo
    file.open("binTest.b", std::fstream::out | std::fstream::binary);

    if(!file.is_open()){
        file.close();

        std::ofstream aux { "binTest.b" };
        aux.close();

        file.open("binTest.b", std::fstream::out | std::fstream::binary);
    }

    file.write(one_w.data(), one_w.size());
    file.close();

    //lectura desde archivo

    file.open("binTest.b", std::fstream::in | std::fstream::binary);

    size_t s = one_w.size();
    char* dat = (char*) malloc (s);
    file.read(dat, s);
    one_w.from_bin(dat);

    file.close();

    return 0;
}
