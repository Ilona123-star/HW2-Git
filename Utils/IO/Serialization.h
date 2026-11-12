#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <string>
#include <type_traits>
#include <stdexcept>
#include <system_error>
#include <cstring>
#include <cerrno>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

namespace I0
{
    template <typename T>
    void Serialize(const T& obj, const std::string& filename)
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "T must be trivially copyable");
        int fd = open(filename.c_str(),O_RDWR | O_CREAT | O_TRUNC,0666);
        if (fd == -1)
            throw std::system_error(errno,std::generic_category(),"en failed");
        if (ftruncate(fd,sizeof(T))==-1)
        {
            int error = errno;
            close(fd);
            throw std::system_error(error, std::generic_category(),"ftruncate failed");
        }
        void* memory = mmap(nullptr,sizeof(T),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
        if (memory == MAP_FAILED)
        {
            int error = errno;
            close(fd);
            throw std::system_error(error, std::generic_category(),"mmap failed");
        }

        std::memcpy(memory, &obj,sizeof(T));
        if (msync(memory,sizeof(T),MS_SYNC) == -1)
        {
            int error = errno;
            munmap(memory,sizeof(T));
            close(fd);
            throw std::system_error(error, std::generic_category(),"msync failed");
        }

        munmap(memory,sizeof(T));
        close(fd);
    }

    template <typename T>
    T Deserialize(const std::string& filename)
    {
        static_assert(std::is_trivially_copyable_v<T>,"T must be trivially copyable");

        int fd = open(filename.c_str(),O_RDONLY);

        if (fd == -1)
            throw std::system_error(errno,std::generic_category(),"open failed");

        struct stat fileInfo{};

        if (fstat(fd,&fileInfo)==-1)
        {
            int error = errno;
            close(fd);
            throw std::system_error(error, std::generic_category(),"fstat failed");
        }
        if (fileInfo.st_size != sizeof(T))
        {
            close(fd);
            throw std::runtime_error("Invalid file size");
        }

        void* memory = mmap(nullptr,sizeof(T),PROT_READ,MAP_PRIVATE,fd,0);
        if (memory == MAP_FAILED)
        {
            int error = errno;
            close(fd);
            throw std::system_error(error, std::generic_category(),"mmap failed");
        }
        T obj{};
        std::memcpy(&obj,memory,sizeof(T));

        munmap(memory,sizeof(T));
        close(fd);

        return obj;
    }
}
#endif
