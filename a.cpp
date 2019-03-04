//Deven Damiano - dad152@zips.uakron.edu - Algorithms P3
//!!!!!!!!!!!!!  DUE TUESDAY DECEMBER 4th !!!!!!!!!!!!

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stdexcept>
#include <cstdlib>

std::vector< std::vector<int> > read_image(std::string, int, int);
std::vector< std::vector<int> > construct_e_matrix(std::vector< std::vector<int> >);
std::vector< std::vector<int> > construct_ce_matrix(std::vector< std::vector<int> >);
std::vector< std::vector<int> > tranpose(std::vector< std::vector<int> >);
std::vector< std::vector<int> > construct_final_matrix(std::vector< std::vector<int> >, std::vector< std::vector<int> >);
std::vector< std::vector<int> > cycle_vertical_seams(std::vector< std::vector<int> >, int);
void export_modified(std::vector< std::vector<int> >, std::string);

std::vector< std::vector<int> > pop_seam(std::vector< std::vector<int> >, std::vector< std::vector<int> >, int, int);
int string_to_int(std::string);
int calc_energy(int, int, int, int, int, std::string);
int find_highest_value(std::vector< std::vector<int> >);
int find_minimum_value_index(std::vector<int>);
bool argument_validator(int, char*[]);

int main(int argc, char* argv[])
{
    std::vector< std::vector<int> > grey_vect;
    std::vector< std::vector<int> > tranposed_matrix;
    std::vector< std::vector<int> > finished_matrix;
    
    // Make sure argument format is correct
    
    if(!argument_validator(argc, argv))
    {
        return 0;
    }
    
    std::string filename = argv[1];
    int v_seams = atoi(argv[2]);
    int h_seams = atoi(argv[3]);
    
    std::cout << v_seams << std::endl;
    std::cout << h_seams << std::endl;
    
    // Read in image
    
    grey_vect = read_image(filename, v_seams, h_seams);
    
    // Find vertical seams
    
    finished_matrix = cycle_vertical_seams(grey_vect, v_seams);
    
    //Transpose for horizontal
    
    tranposed_matrix = tranpose(finished_matrix);
    
    // Find horizontal seams
    
    finished_matrix = cycle_vertical_seams(tranposed_matrix, h_seams);
    
    //Transpose back to original dimensions
    
    finished_matrix = tranpose(finished_matrix);
    
    // Export finished original matrix
    
    export_modified(finished_matrix, filename);
    
    return 0;
}

std::vector< std::vector<int> > read_image(std::string filename, int v_seams, int h_seams)
{
    std::string str;
    std::string value_str;
    
    std::vector< std::vector<int> > grey_vect;
    std::vector<int> values_vect;

    int x;
    int y;
    
    int max_grey;
    
    std::string x_str;
    std::string y_str;
    
    bool past_dimensions = false;
    bool past_max_grey = false;
    bool stored_vect = false;
    
    std::ifstream file;
    
    file.open(filename);
    
    while(!file.eof())
    {
        getline(file, str);
        
        if(past_max_grey)
        {
            value_str = "";
            
            for(int i = 0; i < str.length(); i++)
            {
                if(i == str.length() - 1)
                {
                    value_str += str[i];
                    
                    int value = string_to_int(value_str);
                    
                    values_vect.push_back(value);
                    
                    value_str = "";
                }
                else if(isspace(str[i]) || str[i] == '\t')
                {
                    int value = string_to_int(value_str);
                    
                    values_vect.push_back(value);
                    
                    value_str = "";
                }
                else
                {
                    value_str += str[i];
                }
            }
        }
    
        if(!past_max_grey)
        {
            for (int i = 0; i < str.length(); i++)
            {
                if(isalpha(str[i]))
                {
                    break;
                }
                else if(str[i] == '#' || str[i] == '.' || str[i] == '\n')
                {
                    break;
                }
                
                if(isdigit(str[i]) && !past_dimensions && i == str.length() - 1)
                {
                    value_str += str[i];
                    y = string_to_int(value_str);
                    past_dimensions = true;
                    value_str = "";
                }
                else if(isspace(str[i]) && !past_dimensions && i == str.length() - 1)
                {
                    y = string_to_int(value_str);
                    past_dimensions = true;
                    value_str = "";
                    break;
                }
                else if(isspace(str[i]) && !past_dimensions)
                {
                    x = string_to_int(value_str);
                    value_str = "";
                }
                else if(isdigit(str[i]) && !past_dimensions)
                {
                    value_str += str[i];
                }
                else if(past_dimensions && !past_max_grey && i == str.length() - 1)
                {
                    value_str += str[i];
                    max_grey = string_to_int(value_str);
                    past_max_grey = true;
                    value_str = "";
                }
                else if(past_dimensions && !past_max_grey)
                {
                    value_str += str[i];
                }
            }
        }
        
    }
    
    file.close();
    
    if(v_seams >= x)
    {
        std::cout << "WARNING: Verical seams to be removed is >= x dimension of image, likely cause seg_fault..." << std::endl;
    }
    else if(h_seams >= y)
    {
        std::cout << "WARNING: Horizontal seams to be removed is >= x dimension of image, likely to cause seg_fault..." << std::endl;
    }
    
    //std::cout << values_vect.size() << std::endl;
    //std::cout << x * y << std::endl;
    
    int counter = 0;
    
    std::vector<int> row;
    
    for (int i = 0; i < values_vect.size(); i++)
    {
        counter++;
        
        row.push_back(values_vect[i]);
        
        if(counter == x)
        {
            grey_vect.push_back(row);
            row.clear();
            counter = 0;
        }
    }
    
    return grey_vect;
}

std::vector< std::vector<int> > cycle_vertical_seams(std::vector< std::vector<int> > starting_matrix, int times)
{
    std::vector< std::vector<int> > energy_matrix;
    std::vector< std::vector<int> > ce_energy_matrix;
    std::vector< std::vector<int> > finished_matrix;
    
    if(times > 0) //How many seams do we want to remove
    {
        //construct energy matrix
        
        energy_matrix = construct_e_matrix(starting_matrix);
        
        //construct c_energy matrix
        
        ce_energy_matrix = construct_ce_matrix(energy_matrix);
        
        //construct shrunken matrix
        
        finished_matrix = construct_final_matrix(ce_energy_matrix, starting_matrix);
        
        //recursive, will call until times == 0
        
        starting_matrix = cycle_vertical_seams(finished_matrix, times - 1);
    }
    else
    {
        return starting_matrix;
    }
    
    return starting_matrix;
}

std::vector< std::vector<int> > construct_e_matrix(std::vector< std::vector<int> > grey_vect)
{
    std::vector< std::vector<int> > energy_vect = grey_vect; //allocate needed space
    
    int self, right, left, top, bottom;
    std::string type;
    
    for (int i = 0; i < grey_vect.size(); i++)
     {
         for(int j = 0; j < grey_vect[i].size(); j++)
         {
             self = grey_vect[i][j];
             
             if(i == 0 && j == 0) //top left corner
             {
                 type = "top_left";
                 right = grey_vect[i][j+1];
                 bottom = grey_vect[i+1][j];
             }
             else if(i == 0 && j == grey_vect[i].size() - 1) //top right corner
             {
                 type = "top_right";
                 left = grey_vect[i][j-1];
                 bottom = grey_vect[i+1][j];
             }
             else if(i == 0) //top
             {
                 type = "top";
                 left = grey_vect[i][j-1];
                 right = grey_vect[i][j+1];
                 bottom = grey_vect[i+1][j];
             }
             else if(i == grey_vect.size() - 1 && j == 0) // bottom left corner
             {
                 type = "bottom_left";
                 right = grey_vect[i][j+1];
                 top = grey_vect[i-1][j];
             }
             else if(i == grey_vect.size() - 1 && j == grey_vect[i].size() - 1) // bottom right corner
             {
                 type = "bottom_right";
                 left = grey_vect[i][j-1];
                 top = grey_vect[i-1][j];
             }
             else if(i == grey_vect.size() - 1) // bottom
             {
                 type = "bottom";
                 left = grey_vect[i][j-1];
                 right = grey_vect[i][j+1];
                 top = grey_vect[i-1][j];
             }
             else if(j == 0) // left side
             {
                 type = "left";
                 right = grey_vect[i][j+1];
                 top = grey_vect[i-1][j];
                 bottom = grey_vect[i+1][j];
             }
             else if(j == grey_vect[i].size() - 1) //right side
             {
                 type = "right";
                 left = grey_vect[i][j-1];
                 top = grey_vect[i-1][j];
                 bottom = grey_vect[i+1][j];
             }
             else
             {
                 type = "middle";
                 left = grey_vect[i][j-1];
                 right = grey_vect[i][j+1];
                 top = grey_vect[i-1][j];
                 bottom = grey_vect[i+1][j];
             }
            
            energy_vect[i][j] = calc_energy(self, left, right, top, bottom, type);
             
         }
    }
    
    return energy_vect;
}

std::vector< std::vector<int> > construct_ce_matrix(std::vector< std::vector<int> > e_matrix)
{
    std::vector< std::vector<int> > ce_matrix = e_matrix;
    //int index = 0;
    
    for(int i = 0; i < e_matrix.size(); i++)
    {
        for(int j = 0; j < e_matrix[i].size(); j++)
        {
            if(i == 0)
            {
                //Do nothing
            }
            else
            {
                
                if(j == 0) //left side
                {
                    if(ce_matrix[i-1][j] <= ce_matrix[i-1][j+1])
                    {
                        ce_matrix[i][j] += ce_matrix[i-1][j];
                    }
                    else
                    {
                        ce_matrix[i][j] += ce_matrix[i-1][j+1];
                    }
                }
                else if(j == ce_matrix[i].size() - 1) //right side
                {
                    if(ce_matrix[i-1][j-1] <= ce_matrix[i-1][j])
                    {
                        ce_matrix[i][j] += ce_matrix[i-1][j-1];
                    }
                    else
                    {
                        ce_matrix[i][j] += ce_matrix[i-1][j];
                    }
                }
                else //middle
                {
                    if(ce_matrix[i-1][j-1] <= ce_matrix[i-1][j] && ce_matrix[i-1][j-1] <= ce_matrix[i-1][j+1])
                    {
                        ce_matrix[i][j] += ce_matrix[i-1][j-1];
                    }
                    else if(ce_matrix[i-1][j] <= ce_matrix[i-1][j+1])
                    {
                        ce_matrix[i][j] += ce_matrix[i-1][j];
                    }
                    else
                    {
                        ce_matrix[i][j] += ce_matrix[i-1][j+1];
                    }
                }
                
            }
        }
    }
    
    return ce_matrix;
}

std::vector< std::vector<int> > construct_final_matrix(std::vector< std::vector<int> > ce_matrix, std::vector< std::vector<int> > original_matrix)
{
    std::vector< std::vector<int> > finished_matrix;
    
    int i = ce_matrix.size() - 1;  //start from bottom row
    int j = find_minimum_value_index(ce_matrix[i]); //start from minimum ce_energy in last row
    
    //now begin popping the needed values
            
    original_matrix = pop_seam(ce_matrix, original_matrix, i, j);
    
    return original_matrix;
}

int calc_energy(int self, int left, int right, int top, int bottom, std::string type)
{
    int energy = abs(self - left) + abs(self - right) + abs(self - top) + abs(self - bottom);
    
    if(type == "top_left")
    {
        energy = abs(self - right) + abs(self - bottom);
    }
    else if(type == "top_right")
    {
        energy = abs(self - left) + abs(self - bottom);
    }
    else if(type == "top")
    {
        energy = abs(self - left) + abs(self - right) + abs(self - bottom);
    }
    else if(type == "bottom_left")
    {
        energy = abs(self - right) + abs(self - top);
    }
    else if(type == "bottom_right")
    {
        energy = abs(self - left) + abs(self - top);
    }
    else if(type == "bottom")
    {
        energy = abs(self - left) + abs(self - right) + abs(self - top);
    }
    else if(type == "left")
    {
        energy = abs(self - right) + abs(self - top) + abs(self - bottom);
    }
    else if(type == "right")
    {
        energy = abs(self - left) + abs(self - top) + abs(self - bottom);
    }
    else
    {
        energy = abs(self - left) + abs(self - right) + abs(self - top) + abs(self - bottom);
    }
    
    return energy;
}

std::vector< std::vector<int> > pop_seam(std::vector< std::vector<int> > ce_matrix, std::vector< std::vector<int> > original_matrix, int i, int j)
{
    original_matrix[i].erase(original_matrix[i].begin() + j);
    
    if(i > 0)
    {
        //remove original pixel at ce pixel index
        
        if(j == 0) //left side
        {
            if(ce_matrix[i-1][j] <= ce_matrix[i-1][j+1])
            {
                original_matrix = pop_seam(ce_matrix, original_matrix, i - 1, j);
            }
            else
            {
                original_matrix = pop_seam(ce_matrix, original_matrix, i - 1, j + 1);
            }
        }
        else if(j == ce_matrix[i].size() - 1) //right side
        {
            if(ce_matrix[i-1][j-1] <= ce_matrix[i-1][j])
            {
                original_matrix = pop_seam(ce_matrix, original_matrix, i - 1, j - 1);
            }
            else
            {
                original_matrix = pop_seam(ce_matrix, original_matrix, i - 1, j);
            }
        }
        else // middle
        {
            if(ce_matrix[i-1][j-1] <= ce_matrix[i-1][j] && ce_matrix[i-1][j-1] <= ce_matrix[i-1][j+1]) // left
            {
                original_matrix = pop_seam(ce_matrix, original_matrix, i - 1, j - 1);
            }
            else if(ce_matrix[i-1][j] <= ce_matrix[i-1][j+1]) // middle
            {
                original_matrix = pop_seam(ce_matrix, original_matrix, i - 1, j);
            }
            else //right
            {
                original_matrix = pop_seam(ce_matrix, original_matrix, i - 1, j + 1);
            }
        }
    }
    else
    {
        return original_matrix;
    }
    
    return original_matrix;
}

void export_modified(std::vector< std::vector<int> > finished_vect, std::string filename)
{
    std::size_t index = filename.find('.');
    
    filename.insert(index, "_processed");
    
    std::cout << "Processed file saved as: '" << filename << "'" << std::endl;
    
    std::ofstream out_file (filename);
    
    out_file << "P2" << std::endl;
    
    out_file << finished_vect[0].size() << " " << finished_vect.size() << std::endl;
    
    //std::cout << finished_vect[0].size() << " " << finished_vect.size() << std::endl;
    
    //Find the lowest greatest value in the finished matrix
    
    out_file << find_highest_value(finished_vect) << std::endl;
    
    for(int i = 0; i < finished_vect.size(); i++)
    {
        for(int j = 0; j < finished_vect[i].size(); j++)
        {
            if (j == (finished_vect[i].size()) - 1)
            {
                out_file << finished_vect[i][j] << std::endl;
            }
            else
            {
                out_file << finished_vect[i][j] << " ";
            }
        }
    }
}

int find_highest_value(std::vector< std::vector<int> > matrix)
{
    int highest = 0;
    
    for(int i = 0; i < matrix.size(); i++)
    {
        for(int j = 0; j < matrix[i].size(); j++)
        {
            if(matrix[i][j] > highest)
            {
                highest = matrix[i][j];
            }
        }
    }
    
    return highest;
}

int find_minimum_value_index(std::vector<int> row)
{
    int minimum = row[0];
    int index = 0;
    
    for(int i = 0; i < row.size(); i++)
    {
        if(row[i] < minimum)
        {
            minimum = row[i];
            index = i;
        }
    }
    
    return index;
}

int string_to_int(std::string str)
{
    std::stringstream stream(str);
    
    int x;
    
    stream >> x;
    
    return x;
}

std::vector< std::vector<int> > tranpose(std::vector< std::vector<int> > original_matrix)
{
    std::vector< std::vector<int> > tranposed_matrix(original_matrix[0].size(), std::vector<int>(original_matrix.size()));
    
    for (int i = 0; i < original_matrix.size(); i++)
    {
        for(int j = 0; j < original_matrix[i].size(); j++)
        {
            tranposed_matrix[j][i] = original_matrix[i][j];
        }
    }
    
    return tranposed_matrix;
}

bool argument_validator(int argc, char* argv[])
{
    std::ifstream file;
    
    file.open(argv[1]);
    
    if(argc != 4)
    {
        std::cout << "Format should be './a filename.pgm x y' " << std::endl;
        return false;
    }
    
    if(!file)
    {
        std::cout << "File not found.." << std::endl;
        return false;
    }
    
    if(!isdigit(*argv[2]) || !isdigit(*argv[3]))
    {
        std::cout << "Vertical and Horizontal seam count must be horiontal" << std::endl;
        return false;
    }
    
    return true;
}
