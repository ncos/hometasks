#include "File_sort_functions.h"



void vclear(std::vector<int> &v)
{
    for (int i = 0; i < v.size(); ++i) v[i] = 0;
};
    
    
    
int minnum(std::vector<int> &arr) //Returns number of the minimum element in "arr"
{
    int m = 0;
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] < arr[m])
        {
            m = i;
        };
    };
    return m;
};



int64 fsize(std::ifstream myfile)  //Helps to know the size of a file
{
    myfile.seekg (0, std::ios::beg);
    std::streampos begin = myfile.tellg();
    myfile.seekg (0, std::ios::end);
    std::streampos end = myfile.tellg();
    myfile.seekg (0, std::ios::beg);
    return (int64)(end - begin);
};

std::string generate_path(std::string &path, int &n)
{
    char buf[256];
    sprintf (buf, "%s%d", path.c_str(), n);
    std::string result = buf;
    return result;
};
    
    
    
