// generate ASCII fonts (0x20 ~ 0x7F 96 chars) file from the RAW of
// speciatial size

#include <stdio.h>
#include <stdlib.h>

void main(int argc, char **argv)
{
    FILE *infile, *outfile;
    int  height, width;

    int unpackedsize;
    char *unpacked;
    int l, c, p, h, w;
    int ch;

    printf("GENASCII 1.00     Generate ASCII Font by Yann Tzou\n");

    if (argc < 3)
    {
        printf("Usage: %s inputfile outputfile\n", argv[0]);
        exit(1);
    }

    if ((outfile = fopen(argv[2], "wb")) == NULL)
    {
        printf("Error: can't write %s\n", argv[1]);
        exit(1);
    }

    if ((infile = fopen(argv[1], "rb")) == NULL)
    {
        printf("Error: can't write %s\n", argv[1]);
        exit(1);
    }

    height = 12;    // character pixels
    width  = 6;     // character pixels
    unpackedsize   = ((height * width + 7) / 8) * 8;

    unpacked = (char *) malloc(unpackedsize);

    if (unpacked == NULL)
    {
        printf("Error: not enough memory %d\n", unpackedsize);
        exit(1);
    }

    for (l = 0; l < 4; l++)         // line 0-3
        for (c = 0; c < 24; c++)    // column 0-23
        {
            p = l * width * 24 * height + c * width + 4;

            for (h = 0; h < height; h++)
                for (w = 0; w < width; w++)
                {
                    fseek(infile , p + h * width * 24 + w, SEEK_SET);
                    ch = fgetc(infile);
                    unpacked[h * width + w] = ch;
                }

            for (h = 0; h < unpackedsize; h += 8)
            {
                ch = 0;

                for (w = 0; w < 8; w++)
                    ch |= ((unpacked[h + w] ? 0 : 1) << (7 - w));

                fputc(ch, outfile);
            }
        }

    free(unpacked);

    fclose(infile);
    fclose(outfile);
}
