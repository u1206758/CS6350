#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
Import tree
Import data (except labels)
    for each dataset instance
        start at branch 0
        if current branch has a label
            assign the label to the dataset
        else
            move to the branch where currentbranch.leaf[i].value == instance value for currentbranch.attribute
*/

int countData(void);
int importData(int data[][NUM_ATTRIBUTES+1], int numInstances);

int main()
{

}


int countData(void)
{
     int count = 0;

    //Open input file
    FILE *inputFile = fopen("tennis.csv", "r");
    if (inputFile == NULL)
    {
        printf("Error opening file");
        return -1;
    }

    char row[100];

    //Count number of instances in input file
    while (feof(inputFile) != true)
    {
        if (fgets(row, 100, inputFile) == NULL)
        {
            fclose(inputFile);
            return count;
        }
        else
        {
            count++;
        }
    }
    fclose(inputFile);
    return count;
}

int importData(int data[][NUM_ATTRIBUTES+1], int numInstances)
{
    FILE *inputFile = fopen("tennis.csv", "r");
    if (inputFile == NULL)
    {
        printf("Error opening file");
        return -1;
    }

    char row[100];
    char *token;

    //Parse input CSV into data instance struct array
    while (feof(inputFile) != true)
    {
        for (int i = 0; i < numInstances; i++)
        {
            if (fgets(row, 100, inputFile) == NULL)
            {
                fclose(inputFile);
                return 0;
            }
            else
            {
                token = strtok(row, ",");
                for (int j = 0; j < NUM_ATTRIBUTES+1; j++)
                {
                    data[i][j] = valueToInt(token);
                    token = strtok(NULL, ",\r\n");
                }
            }
        }
    }
    fclose(inputFile);
    return 0;
}
