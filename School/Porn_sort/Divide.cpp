#include "Divide.h"
/*
void save_next_part(FILE* f, int n, unsigned long position, unsigned long fsz, char where_file[64], int dsize) //Generates the sorted part of a big file "f"; "n" is a number of file
{
    std::vector<int> storage;
    
    //Creating path
    std::string final_path = generate_path(where_file, n);
    //"final_path" stores path to the buffer file
    
    std::cout << final_path << " - here the buffer data stores\n";
    std::cout << "All buffer data will be deleted after merging automatically.\n";
    FILE *part = fopen(final_path, "w");
    
    
    
    unsigned long i = 0;
    while ((i < dsize) && (position + i < fsz))
    {   
        i++;
        int buf;
        fread(&buf, sizeof(int), 1, f);
        storage.push_back(buf);
    };
    
    
    stable_sort (storage.begin(), storage.end());

    
    fwrite(&storage[0], sizeof(int), storage.size(), part);
    fclose(part);
};
*/

int divide_and_sort(std::string &input_path, std::string &buf_path, int &dsize) //Generate "n_of_parts" sorted parts of the file "f" 
{
    std::ifstream myfile (input_path.c_str(), std::ios::in | std::ios::binary);
    if (myfile.is_open())
    {
        int64 filesize = fsize(myfile);
        int64 nnum = ceil((double)filesize / (double)sizeof(int));
        int64 n_of_parts = ceil((double)filesize / (double)dsize);

        std::cout << "Sucsessfully opened " << input_path 
                  << " (" << filesize << " bytes)" 
                  << "\nFull number of integers: " << nnumn
                  << "\nThe number of files: " << n_of_parts 
                  << std::endl << std::endl;
        
        char * memblock = = new char [dsize * sizeof(int)];
        for (int n = 0; n < n_of_parts; ++n) {
            std::string part_path = generate_path(buf_path, n);
            std::ofstream part (part_path.c_str(), std::ios::out | std::ios::binary);
            if (part.is_open())
            {
                std::cout << "Sucsessfully opened " << part_path << std::endl; 
                myfile.read(memblock, dsize * sizeof(int));
                part.write(memblock, myfile.gcount());
                std::cout << "\tWritten " << myfile.gcount() << " characters\n";
            }
            else
            {
                delete[] memblock;
                myfile.close();
                std::cout << "Unable to open file" << part_path << std::endl;
                std::exit(-1);
            }
        }
        
        delete[] memblock;
        myfile.close();
        return n_of_parts; //Returns number of files
    }
    else {
        std::cout << "Unable to open file" << input_path << std::endl;
        std::exit(-1);
    }
};

