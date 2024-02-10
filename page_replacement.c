#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define UNFILLED_FRAME -1
#define MAX_QUEUE_SIZE 10
#define MAX_FRAMES_AVAILABLE 10
#define MAX_REFERENCE_STRING 30

#define ALGORITHM_FIFO "FIFO"
#define ALGORITHM_OPT "OPT"
#define ALGORITHM_LRU "LRU"
#define ALGORITHM_CLOCK "CLOCK"

#define KEYWORD_ALGORITHM "algorithm"
#define KEYWORD_FRAMES_AVAILABLE "frames_available"
#define KEYWORD_REFERENCE_STRING_LENGTH "reference_string_length"
#define KEYWORD_REFERENCE_STRING "reference_string"


#define SPACE_CHARS " \t"

char algorithm[10];
int reference_string[MAX_REFERENCE_STRING];
int reference_string_length;
int frames_available;
int frames[MAX_FRAMES_AVAILABLE];

int is_blank(char *line)
{
    char *ch = line;
    while (*ch != '\0')
    {
        if (!isspace(*ch))
            return 0;
        ch++;
    }
    return 1;
}

int is_skip(char *line)
{
    if (is_blank(line))
        return 1;
    char *ch = line;
    while (*ch != '\0')
    {
        if (!isspace(*ch) && *ch == '#')
            return 1;
        ch++;
    }
    return 0;
}

void parse_tokens(char **argv, char *line, int *numTokens, char *delimiter)
{
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc;
}


void parse_input()
{
    FILE *fp = stdin;
    char *line = NULL;
    ssize_t nread;
    size_t len = 0;

    char *two_tokens[2];                                 // buffer for 2 tokens
    char *reference_string_tokens[MAX_REFERENCE_STRING]; // buffer for the reference string
    int numTokens = 0, n = 0, i = 0;
    char equal_plus_spaces_delimiters[5] = "";

    strcpy(equal_plus_spaces_delimiters, "=");
    strcat(equal_plus_spaces_delimiters, SPACE_CHARS);

    while ((nread = getline(&line, &len, fp)) != -1)
    {
        if (is_skip(line) == 0)
        {
            line = strtok(line, "\n");
            if (strstr(line, KEYWORD_ALGORITHM))
            {
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2)
                {
                    strcpy(algorithm, two_tokens[1]);
                }
            }
            else if (strstr(line, KEYWORD_FRAMES_AVAILABLE))
            {
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2)
                {
                    sscanf(two_tokens[1], "%d", &frames_available);
                }
            }
            else if (strstr(line, KEYWORD_REFERENCE_STRING_LENGTH))
            {
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2)
                {
                    sscanf(two_tokens[1], "%d", &reference_string_length);
                }
            }
            else if (strstr(line, KEYWORD_REFERENCE_STRING))
            {
                parse_tokens(two_tokens, line, &numTokens, "=");
                if (numTokens == 2)
                {
                    parse_tokens(reference_string_tokens, two_tokens[1], &n, SPACE_CHARS);
                    for (i = 0; i < n; i++)
                    {
                        sscanf(reference_string_tokens[i], "%d", &reference_string[i]);
                    }
                }
            }
        }
    }
}

void print_parsed_values()
{
    int i;
    printf("%s = %s\n", KEYWORD_ALGORITHM, algorithm);
    printf("%s = %d\n", KEYWORD_FRAMES_AVAILABLE, frames_available);
    printf("%s = %d\n", KEYWORD_REFERENCE_STRING_LENGTH, reference_string_length);
    printf("%s = ", KEYWORD_REFERENCE_STRING);
    for (i = 0; i < reference_string_length; i++)
        printf("%d ", reference_string[i]);
    printf("\n");
}



const char template_total_page_fault[] = "Total Page Fault: %d\n";
const char template_no_page_fault[] = "%d: No Page Fault\n";


void display_fault_frame(int current_frame)
{
    int j;
    printf("%d: ", current_frame);
    for (j = 0; j < frames_available; j++)
    {
        if (frames[j] != UNFILLED_FRAME)
            printf("%d ", frames[j]);
        else
            printf("  ");
    }
    printf("\n");
}

void frames_init()
{
    int i;
    for (i = 0; i < frames_available; i++)
        frames[i] = UNFILLED_FRAME;
}

void FIFO_replacement()
{
    int numPageFaults = 0;
    int counter = 0;
    
    for(int i = 0; i < reference_string_length; i++)
    {
      bool PF = true;
      for(int j = 0; j < frames_available; j++)
        if(frames[j] == reference_string[i])
          PF = false;
      if(PF)
      {
        numPageFaults++;
        frames[counter] = reference_string[i];
        counter = (counter + 1) % frames_available;
        display_fault_frame(reference_string[i]);
      }
      else
        printf(template_no_page_fault, reference_string[i]);
      if(i == reference_string_length-1)
        printf(template_total_page_fault, numPageFaults);
    }
    
}

void OPT_replacement()
{
    int numPageFaults = 0;
    for(int i = 0; i < reference_string_length; i++)
    {
      bool PF = true;

      int furthestFrame;
      int furthestFrameOffset = -1;

      for(int j = 0; j < frames_available; j++)
        if(frames[j] == reference_string[i])
          PF = false;

      if(PF)
      {
        numPageFaults++;
        for(int j = 0; j < frames_available; j++)
        {
          if(frames[j] == -1)
          {
            furthestFrame = j;
            break;
          }
          int offset = 0;
          for(int k = i; k < reference_string_length; k++)
          {
            if(frames[j] == reference_string[k] && i != k)
            {
              offset = k-i;
              if(offset > furthestFrameOffset && furthestFrameOffset != -2)
              {
                furthestFrameOffset = offset;
                furthestFrame = j;
              }
              break;
            }
          }
          if(offset == 0)
          {
            if((furthestFrameOffset==-2 && frames[j] < frames[furthestFrame]) || (furthestFrameOffset != -2))
            {
              furthestFrame = j;
              furthestFrameOffset = -2;
            }
          }
        }

        frames[furthestFrame] = reference_string[i];
        display_fault_frame(reference_string[i]);
      }
      else
        printf(template_no_page_fault, reference_string[i]);
      if(i == reference_string_length-1)
        printf(template_total_page_fault, numPageFaults);
    }
}

void LRU_replacement()
{
    int frameCounter[10] = {0};
    int numPageFaults = 0;
    for(int i = 0; i < reference_string_length; i++)
    {
        bool PF = true;
        int LRU = reference_string_length;
        int LRUFrame;

        for(int j = 0; j < frames_available; j++)
            if(frames[j] == reference_string[i])
                PF = false;

        if(PF)
        {
            numPageFaults++;
            for(int j = 0; j < frames_available; j++)
            {
                if(frames[j] == -1)
                {
                    LRUFrame = j;
                    break;
                }
                if(frameCounter[frames[j]] < LRU)
                {
                    LRU = frameCounter[frames[j]];
                    LRUFrame = j;
                }
            }
            frames[LRUFrame] = reference_string[i];
            frameCounter[reference_string[i]] = i; 
            display_fault_frame(reference_string[i]);
        }
        else
        {
            frameCounter[reference_string[i]] = i;
            printf(template_no_page_fault, reference_string[i]);
        }
        if(i == reference_string_length-1)
            printf(template_total_page_fault, numPageFaults);
    }
}

void CLOCK_replacement()
{
    int second_chance[frames_available];
    for(int i = 0; i< frames_available; i++)
    {
        second_chance[i] = 0;
    }
    int numPageFaults = 0;
    int pointer = 0;

    for(int i = 0; i < reference_string_length; i++)
    {
        bool PF = true;
        int frameIdx;

        for(int j = 0; j < frames_available; j++)
        {
            if(frames[j] == reference_string[i])
            {
                PF = false;
                frameIdx = j;
            }
        }
        
        if(PF)
        {
            numPageFaults++;
            int replacedFrame;
            bool initialFrame = false;

            for(int j = 0; j < frames_available; j++)
            {
                if(frames[j] == -1)
                {
                    replacedFrame = j;
                    initialFrame = true;
                    break;
                }
            }
            if(!initialFrame)
            {
                while(true)
                {
                    if(second_chance[pointer] == 1)
                    {
                        second_chance[pointer] = 0;
                        pointer = (pointer + 1) % frames_available;
                    }
                    else
                    {
                        replacedFrame = pointer;
                        pointer = (pointer + 1) % frames_available;
                        break;
                    }
                }
            }
            frames[replacedFrame] = reference_string[i];
            display_fault_frame(reference_string[i]);
        }
        else
        {
            second_chance[frameIdx] = 1;
            printf(template_no_page_fault, reference_string[i]);
        }
        if(i == reference_string_length-1)
            printf(template_total_page_fault, numPageFaults);
    }
}

int main()
{
    parse_input();              
    print_parsed_values();      
    frames_init();              

    if (strcmp(algorithm, ALGORITHM_FIFO) == 0)
    {
        FIFO_replacement();
    }
    else if (strcmp(algorithm, ALGORITHM_OPT) == 0)
    {
        OPT_replacement();
    }
    else if (strcmp(algorithm, ALGORITHM_LRU) == 0)
    {
        LRU_replacement();
    }
    else if (strcmp(algorithm, ALGORITHM_CLOCK) == 0)
    {
        CLOCK_replacement();
    }

    return 0;
}