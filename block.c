#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "font.h"


#define PIXEL_WIDTH (3)
#define PIXEL_STR ";-)"
#define PIXEL_SPC "   "

#define MAX_CHARACTER_WIDTH (PIXEL_WIDTH * 5)
#define LINE_WIDTH (30)
#define MAX_LINES (25)


char* strcat_input(int argc, char* argv[])
{
    int i;
    int size = 0;
    char* tmp;
    
    for(i = 0; i < argc; ++i)
    {
       size += strlen(argv[i]) + 1;
    }

    tmp = malloc(size+1);
    memset(tmp,0,size+1);
    
    for(i = 0; i < argc; ++i)
    {
        strcat(tmp,argv[i]);
        strcat(tmp," ");
    }
    
    tmp[size-1] = 0; //remove the last space and turn it into a null terminator

    
    return tmp;
    
}

int getlines(char* input, int input_length, int width, char** lines)
{
    int num_lines = 0;
    int str_index = 0;
    int line_width = 0;
    int last_space = 0;
    int line_start_index = 0;
    int lastline = 0; //boolean


    while(!lastline)
    {
        lines[num_lines] = malloc(width);
        memset(lines[num_lines],0,width);
        str_index = line_start_index;
        line_width = 0;
        while(line_width <= width) //fill up the entire line
        {
            if(str_index == input_length)
            {
                last_space = str_index;
                lastline = 1; //break out of main loop too
                break;
            }
            
            if(input[str_index] == ' ')
                last_space = str_index;
            line_width += PIXEL_WIDTH*(letter_width[get_font_index(input[str_index])]+1);
            str_index++;
        }

        if(last_space > line_start_index)
        {
            // we've gone past the line width, so store everything up to
            // the last space into this line. If the last space is what caused
            // the line to go over, then the line fits perfectly
            memcpy(lines[num_lines], &input[line_start_index], last_space-line_start_index);
            num_lines++;
            line_start_index = last_space+1;
        }
        else
        {
            // this word won't fit on one line. so grab everything we can 
            // and leave the rest in the next line
            memcpy(lines[num_lines], &input[line_start_index], (str_index-1)-line_start_index);
            num_lines++;
            line_start_index = str_index - 1;
        }
    }
    return num_lines;

}



void substitute_pixels(char** pixel_display, char* input)
{
    int ind;
    int height;
    int pix;
    int font_index;
    int progress_index = 0;
    for(ind = 0; ind < strlen(input); ++ind)
    {
        font_index = get_font_index(input[ind]);
        for(height = 0; height < FONT_HEIGHT; ++height)
        {
            
            for(pix = 0; pix < letter_width[font_index]; ++pix)
            {
                if(font[font_index][height][pix] == '*')
                    strncpy(&pixel_display[height][pix*PIXEL_WIDTH+progress_index],PIXEL_STR, PIXEL_WIDTH);
                else if(font[font_index][height][pix] == ' ')
                    strncpy(&pixel_display[height][pix*PIXEL_WIDTH+progress_index],PIXEL_SPC, PIXEL_WIDTH);
                else
                    printf("Found invalid character '%c' in font index %d", \
                            font[font_index][height][pix] , font_index);
            }
            strncpy(&pixel_display[height][pix*PIXEL_WIDTH+progress_index],PIXEL_SPC, PIXEL_WIDTH); //add a space between letters
            
        }
        progress_index+=(letter_width[font_index]+1)*PIXEL_WIDTH;
    }
}

void pixel_print(char** lines, int num_lines, int max_width)
{
    int i,j;
    char** display = malloc(FONT_HEIGHT* sizeof(char*));

    for(i = 0; i < FONT_HEIGHT; ++i)
    {
        display[i] = malloc(max_width);
        memset(display[i],0,max_width);
    }
    
    for(i = 0; i < num_lines; ++i)
    {
        substitute_pixels(display,lines[i]);
        for(j = 0; j < FONT_HEIGHT; ++j)
        {
            printf("%s\n",display[j]);
            memset(display[j],0,max_width);
        }
        if(i+1!=num_lines) //don't put spaces after last line
            printf("\n\n");
    }
    
    for(i = 0; i < FONT_HEIGHT; ++i)
    {
        free(display[i]);
    }
    
    free(display);
}

int get_terminal_columns()
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

int main(int argc, char *argv[])
{
    char* input_string;
    char** lines = malloc(MAX_LINES);
    int num_lines;
    int i = 0;
    int colwidth = LINE_WIDTH*PIXEL_WIDTH;

#ifndef DEBUG
    colwidth = get_terminal_columns();
#endif
    
    if(colwidth  <= 0)
        colwidth = LINE_WIDTH*PIXEL_WIDTH;
    //remove first argument (program name)

    input_string = strcat_input(argc-1, &argv[1]);
    
    num_lines = getlines(input_string,strlen(input_string), colwidth, lines);

    pixel_print(lines,num_lines,colwidth+1);


    //cleanup
    for(i = 0; i < num_lines; ++i)
    {
        free(lines[i]);
    }

    free(lines);
    free(input_string);
    
    return 0;
}

