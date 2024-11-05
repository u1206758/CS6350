#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define NUM_TRAINING_INSTANCES 872   //872 instances in the training set
#define NUM_TESTING_INSTANCES 500   //500 instances in the test set
#define NUM_LABELS 2        //Binary label
#define NUM_ATTRIBUTES 4    //4 attributes


int import_training_data(float data[][NUM_ATTRIBUTES+1]);
int import_testing_data(float data[][NUM_ATTRIBUTES+1]);

int main()
{
    float training_data[NUM_TRAINING_INSTANCES][NUM_ATTRIBUTES+1];
    float w[4] = {0, 0, 0, 0};
    float b = 0;

    //Import training data from CSV
    if (import_training_data(training_data) == -99)
        return 1;

    //Perceptron linear equation learner
    for (int epoch = 0; epoch < 10; epoch++)
    {
        int updates = 0;
        for (int i = 0; i < NUM_TRAINING_INSTANCES; i++)
        {
            //Calculate prediction with current linear function
            float prediction = w[0]*training_data[i][0]+w[1]*training_data[i][1]+w[2]*training_data[i][2]+w[3]*training_data[i][3]+b;
            //Compare prediction to actual label
            if ((prediction <= 0 && training_data[i][4] == 1) || (prediction > 0 && training_data[i][4] == -1))
            {
                //Update function if necessary
                for (int j = 0; j < NUM_ATTRIBUTES; j++)
                {
                    w[j] += training_data[i][4] * training_data[i][j];
                }
                b += training_data[i][4];
                updates++;
            }
        }
    }

    printf("\nLearning complete\n\n");
    printf("w[0]: %f\n", w[0]);
    printf("w[1]: %f\n", w[1]);
    printf("w[2]: %f\n", w[2]);
    printf("w[3]: %f\n", w[3]);
    printf("b: %f\n", b);

    float testing_data[NUM_TESTING_INSTANCES][NUM_ATTRIBUTES+1];
    int errors = 0;

    //Import testing data from CSV
    if (import_testing_data(testing_data) == -99)
        return 1;

    //Make predictions on testing data using learned function
    for (int i = 0; i < NUM_TESTING_INSTANCES; i++)
    {
        //Calculate prediction with current linear function
        float prediction = w[0]*testing_data[i][0]+w[1]*testing_data[i][1]+w[2]*testing_data[i][2]+w[3]*testing_data[i][3]+b;
        //printf("%f   %f\n", prediction, testing_data[i][4]);
        //Compare prediction to actual label
        if ((prediction <= 0 && testing_data[i][4] == 1) || (prediction > 0 && testing_data[i][4] == -1))
        {
            errors++;
        }
    }

    printf("\nPredicting complete\n");
    printf("\n%d incorrect predictions on %d instances\n", errors, NUM_TESTING_INSTANCES);
    printf("prediction error: %.2f%% \n\n", ((float) errors / (float) NUM_TESTING_INSTANCES)*100);

    return 0;
}

int import_training_data(float data[][NUM_ATTRIBUTES+1])
{
    FILE *inputFile = fopen("train.csv", "r");
    if (inputFile == NULL)
    {
        printf("Error opening file");
        return -99;
    }

    char row[300];
    char *token;
    //Parse input CSV into data instance struct array
    while (feof(inputFile) != true)
    {
        for (int i = 0; i < NUM_TRAINING_INSTANCES; i++)
        {
            if (fgets(row, 300, inputFile) == NULL)
            {
                fclose(inputFile);
                return 0;
            }
            else
            {
                token = strtok(row, ",");
                for (int j = 0; j < NUM_ATTRIBUTES+1; j++)
                {
                    data[i][j] = atof(token);
                    if (j == 4 && data[i][j] == 0)
                    {
                        data[i][j] = -1;
                    }
                    token = strtok(NULL, ",\r\n");
                }
            }
        }
    }
    fclose(inputFile);
    return 0;
}

int import_testing_data(float data[][NUM_ATTRIBUTES+1])
{
    FILE *inputFile = fopen("test.csv", "r");
    if (inputFile == NULL)
    {
        printf("Error opening file");
        return -99;
    }

    char row[300];
    char *token;
    //Parse input CSV into data instance struct array
    while (feof(inputFile) != true)
    {
        for (int i = 0; i < NUM_TESTING_INSTANCES; i++)
        {
            if (fgets(row, 300, inputFile) == NULL)
            {
                fclose(inputFile);
                return 0;
            }
            else
            {
                token = strtok(row, ",");
                for (int j = 0; j < NUM_ATTRIBUTES+1; j++)
                {
                    data[i][j] = atof(token);
                    if (j == 4 && data[i][j] == 0)
                    {
                        data[i][j] = -1;
                    }
                    token = strtok(NULL, ",\r\n");
                }
            }
        }
    }
    fclose(inputFile);
    return 0;
}

