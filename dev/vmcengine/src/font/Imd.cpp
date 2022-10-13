// funcs. for supporting text input method

// Initialize input method
InputMethod.Init(this, 9);

int  Text::ChangeInputMethod(int method)
{
    return InputMethod.ChangeMethod(method);
}

int  Text::InputChinese(int x, int y, char *str)
{
    int  key;

    if (!InputMethod.Display(x, y))
        return 0;

    key = InputMethod.Input(str);
    InputMethod.Restore();

    if (key == ESC)
        return -1;

    return strlen(str);
}

#define TEXT_FG_COLOR   1
#define TEXT_BG_COLOR   115

int  Text::GetsXY(int x, int y, char *orgAnswer, int maxDispCol)
{
    int  firstKey;              // if this is the first input key?
    int  length;
    int  col;
    int  key;
    char answer[80];
    int  FgColor, BgColor;

    GetColor(FgColor, BgColor);
//  SetColor(TEXT_FG_COLOR, TEXT_BG_COLOR);
    SetColor(242, 0);

    strcpy(answer, orgAnswer);
    length = strlen(answer);
    col = length - 1;

    PutsXY(x, y, answer);

    firstKey = 1;
    key = getkey(WAIT);
    if (key >= 32 && key <= 127)
    {
        answer[0] = (char) key;
        answer[1] = '\0';
        col = 0;
        length = 1;
    }

    do
    {
        fillbox(x, y, x + maxDispCol * 8 - 1, y + 14, TEXT_BG_COLOR);
        PutsXY(x, y, answer);
        drawline(x + col * 8, y, x + col * 8, y + 14, -TEXT_BG_COLOR);
        drawline(x + col * 8 + 1, y, x + col * 8 + 1, y + 14, -TEXT_BG_COLOR);

        if (firstKey)
            firstKey = 0;
        else
            key = getkey(WAIT);

        if (key > 0x19 && key < 0x7F)
        {
            answer[col] = (char) key;

            if (++col >= maxDispCol)
            {
                col = maxDispCol;
                answer[col--] = '\0';
            }

            if (col > length)
            {
                length = col;
                answer[col] = '\0';
            }

            continue;
        }

        switch (key)
        {
        case ENTER:
            strcpy(orgAnswer, answer);
            SetColor(FgColor, BgColor);
            return strlen(answer);

        case F1:
            col += InputChinese(x + 16, y + 16, answer + col);
            break;

        case BS:
            if (--col < 0)
                col = 0;
            else
            {
                length--;
                strcpy((answer + col), (answer + col + 1));
            }
            break;

        case LEFTKEY:
            if (--col < 0)
                col = 0;
            break;

        case RIGHTKEY:
            if (++col > length)
                col = length;
            else if (col >= maxDispCol)
                col--;
            break;

        case DELKEY:
            if (col != length)
            {
                strcpy((answer + col), (answer + col + 1));
                length--;
            }
            break;
        }
    } while (key != ESC);

    SetColor(FgColor, BgColor);
    return 0;
}
