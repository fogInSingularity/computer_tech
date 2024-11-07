/* Программа для записи текста исходного файла в разделяемую память, 
   проверить совместнос 2 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* Предполагаем, что размер исходного файла < SIZE */
const size_t kMaxSize = 65535;
const int kFullAccess = 0666;

int main()
{
    const char* file_path = "some_file";

    /* Генерируем IPC ключ из имени файла и номера экземпляра области разделяемой памяти 0 */
    key_t ipc_key = ftok(file_path, 0);
    if (ipc_key < 0) {
        perror("Cant generate key");

        return EXIT_FAILURE;
    }
    
    /* Пытаемся создать разделяемую память для сгенерированного ключа */
    int shared_memory_id = shmget(ipc_key, kMaxSize, kFullAccess | IPC_CREAT); /* IPC дескриптор для области разделяемой памяти */
    if (shared_memory_id < 0) {
        perror("Cant create shared memory");
        
        return EXIT_FAILURE;
    }

    /* Пытаемся отобразить разделяемую память в адресное пространство текущего процесса */ 
    uint8_t* shared_memory = shmat(shared_memory_id, NULL, 0); 
    if (shared_memory == (void*)(-1)) {
        perror("Cant attach shared memory");

        return EXIT_FAILURE;
    }  

    /* Открываем файл только на чтение*/ 
    int file_descr = open(file_path, O_RDONLY);
    if (file_descr < 0) {
        perror("Cant open file");
        shmdt(shared_memory);

        return EXIT_FAILURE;
    }
    
    /* Читаем файл порциями по 1kb до тех пор, пока не достигнем конца файла 
    или не возникнет ошибка */
    size_t kChunkSize = 1024;
    // uint8_t* start_of_data = calloc(kChunkSize, sizeof(uint8_t)); /* Временный указатель для записи фрагментов файла */
    // size_t current_max_size = kChunkSize;
    size_t data_readed = 0;

    uint8_t* move_ptr = shared_memory;

    ssize_t n_bytes_read = 0; 
    while((n_bytes_read = read(file_descr, move_ptr, kChunkSize)) > 0)
    { 
        data_readed += (size_t)n_bytes_read;
        move_ptr += data_readed;
    } 

    fprintf(stderr, "file size: %lu\n", data_readed);

    /* Закрываем файл */ 
    close(file_descr); 

    /* Если возникла ошибка - завершаем работу */ 
    if(n_bytes_read < 0)
    { 
        perror("Error while reading file");
        shmdt(shared_memory); 

        return EXIT_FAILURE; 
    } 

    /* После всего считанного текста вставляем признак конца строки,
    чтобы впоследствии распечатать все одним printf'ом */
    *move_ptr = '\0';

    /* Печатаем содержимое буфера.*/
    // puts(start_of_data);

    puts("Waiting...\n");
    getchar();

    /* Отсоединяем разделяемую память и завершаем работу */ 
    int shmdt_res = shmdt(shared_memory); 
    if(shmdt_res < 0)
    {
        perror("Cant detach shared memory");

        return EXIT_FAILURE;
    }

    return 0;
}
