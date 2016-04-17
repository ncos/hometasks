#include "Merge.h"

std::vector<int> comparison_line(int N, std::vector<int> &buf, std::vector<int> &place, char from_where[10])
{//Takes an element number "place" from "i" file and puts it in "buf[i]";    
    int i = 0;
    int k = 0;
    while (i < buf.size())
    {
    
        int elem;
        k = std::max(i, k);
        char *final_path = generate_path(from_where, k);
        FILE *part = fopen(final_path, "r");
        while (part == NULL) //If file "i" doesn't exist we'll take "i+1" file
        {
            k++;
            fclose(part);
            final_path = generate_path(from_where, k);
            part = fopen(final_path, "r");
            //std::cout<<k-1<<" -f was del. Opening: "<<final_path<<"\n";
        };
        
        
        for (int j = 0; j < place[i]; j++)
        {
            fread(&elem, sizeof(int), 1, part);
        };
        
        
        fread(&elem, sizeof(int), 1, part);
        if (!feof(part))
        {
            buf[i] = elem;            
        }
        else
        {
            buf.erase(buf.begin() + i); //In case if current file was completely procesed just now
            place.erase(place.begin() + i); //We will delete all information about it and also this file
            fclose(part);
            final_path = generate_path(from_where, k);
            remove(final_path);
            k=-1;
            i=-1;       
        };
        
        fclose(part);
        
       
        k++;
        i++;
    };
    
    
    return buf;
};



    

void merge(int N, char *out_path, char buf_path[64])  //Merges all generated files
{
    FILE *output = fopen(out_path, "w+");
    std::vector<int> buf, place;
    place.resize(N);
    buf.resize(N);
    
    vclear(place); //Make all "place" cells zero
    
    
    
    
    while (buf.size() > 0)
    {
        int min_place = minnum(comparison_line(N, buf, place, buf_path));
        
        if(buf.size() > 0) //For not to double the last int in "output"
        {
            fwrite(&buf[min_place], sizeof(int), 1, output);
        };
        
        if ((place[min_place]/100.0)==(place[min_place]/100)) //Approximate progress
        {
            std::cout<<place[min_place]<<"\n";
        };
            
        place[min_place]++;
        
    };
    
    fclose(output);
};
