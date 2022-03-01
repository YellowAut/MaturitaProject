//https://forum.arduino.cc/t/a-simple-function-for-reading-csv-text-files/328608/3

// Example to read a two dimensional array.
//
#include <SPI.h>
#include <SD.h>
#define CS_PIN 10

// 5 X 4 array
#define ROW_DIM 5
#define COL_DIM 4

File file;

/*
 * Read a file one field at a time.
 *
 * file - File to read.
 *
 * str - Character array for the field.
 *
 * size - Size of str array.
 *
 * delim - String containing field delimiters.
 *
 * return - length of field including terminating delimiter.
 *
 * Note, the last character of str will not be a delimiter if
 * a read error occurs, the field is too long, or the file
 * does not end with a delimiter.  Consider this an error
 * if not at end-of-file.
 *
 */
size_t readField(File *file, char *str, size_t size, char *delim)
{
    char ch;
    size_t n = 0;
    while ((n + 1) < size && file->read(&ch, 1) == 1)
    {
        // Delete CR.
        if (ch == '\r')
        {
            continue;
        }
        str[n++] = ch;
        if (strchr(delim, ch))
        {
            break;
        }
    }
    str[n] = '\0';
    return n;
}
//------------------------------------------------------------------------------
#define errorHalt(msg)          \
    {                           \
        Serial.println(F(msg)); \
        while (1)               \
            ;                   \
    }
//------------------------------------------------------------------------------
void setup()
{
    Serial.begin(9600);

    // Initialize the SD.
    if (!SD.begin(CS_PIN))
    {
        errorHalt("begin failed");
    }
    // Create or open the file.
    file = SD.open("READNUM.TXT", FILE_WRITE);
    if (!file)
    {
        errorHalt("open failed");
    }
    // Rewind file so test data is not appended.
    file.seek(0);

    // Write test data.
    file.print(F(
        "11,12,13,14\r\n"
        "21,22,23,24\r\n"
        "31,32,33,34\r\n"
        "41,42,43,44\r\n"
        "51,52,53,54" // Allow missing endl at eof.
        ));

    // Rewind the file for read.
    file.seek(0);

    // Array for data.
    int array[ROW_DIM][COL_DIM];
    int i = 0;      // First array index.
    int j = 0;      // Second array index
    size_t n;       // Length of returned field with delimiter.
    char str[20];   // Must hold longest field with delimiter and zero byte.
    char *ptr;      // Test for valid field.
    char delim = 0; // Delimiter from previous line. Start with no delimiter.

    // Read the file and store the data.
    while (true)
    {
        n = readField(&file, str, sizeof(str), ",\n");

        // Read error or at EOF.
        if (n == 0)
        {
            break;
        }
        // Advance indices based on previous delimiter.
        if (delim == '\n')
        {
            // previous delimiter was endl so start a new row.
            if (++i >= ROW_DIM)
            {
                errorHalt("too many lines");
            }
            if (j != (COL_DIM - 1))
            {
                errorHalt("missing field");
            }
            j = 0;
        }
        else if (delim == ',')
        {
            // previous delimiter was comma so advance column.
            if (++j >= COL_DIM)
            {
                errorHalt("too many fields");
            }
        }
        array[i][j] = strtol(str, &ptr, 10);
        if (ptr == str)
        {
            errorHalt("bad number");
        }
        // Skip any blanks after number.
        while (*ptr == ' ')
        {
            ptr++;
        }
        // Save delimiter.
        delim = *ptr;

        if (delim != ',' && delim != '\n' && delim != 0)
        {
            errorHalt("extra data in field");
        }
        if (delim == 0 && file.available() != 0)
        {
            errorHalt("read error or long line");
        }
    }
    // Check that entire array was read.
    if (j != (COL_DIM - 1) || i != (ROW_DIM - 1))
    {
        errorHalt("missing data");
    }
    // Print the array.
    for (i = 0; i < ROW_DIM; i++)
    {
        for (j = 0; j < COL_DIM; j++)
        {
            if (j)
            {
                Serial.print(' ');
            }
            Serial.print(array[i][j]);
        }
        Serial.println();
    }
    Serial.println("Done");
    file.close();
}
//------------------------------------------------------------------------------
void loop()
{
}