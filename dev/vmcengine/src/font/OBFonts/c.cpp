// Graphic text variables
Text    Ming12;                 // Ming font
Text    Ming14;                 // Ming font
Text    Ming16;                 // Ming font
Text    Cute18;                 // Cute Girl font



BOOL CreateFonts(void)
{
    char  str1[_MAX_PATH];
    char  str2[_MAX_PATH];

    // Load fonts
    makefilename(str1, "Languages\\EM0612.FNT");
    makefilename(str2, "Languages\\CM1212.FNT");

    Ming12.LoadFont(FONT_SET_SBCS, str1,  6, 12, FONT_FMT_0, 1);
    Ming12.LoadFont(FONT_SET_DBCS, str2, 12, 12, FONT_FMT_0, 1);

    makefilename(str1, "Languages\\EM0714.FNT");
    makefilename(str2, "Languages\\CM1414.FNT");

    Ming14.LoadFont(FONT_SET_SBCS, str1,  7, 14, FONT_FMT_0, 1);
    Ming14.LoadFont(FONT_SET_DBCS, str2, 14, 14, FONT_FMT_0, 1);

    makefilename(str1, "Languages\\EM0816.FNT");
    makefilename(str2, "Languages\\CM1616.FNT");

    Ming16.LoadFont(FONT_SET_SBCS, str1,  8, 16, FONT_FMT_0, 1);
    Ming16.LoadFont(FONT_SET_DBCS, str2, 16, 16, FONT_FMT_0, 1);

    makefilename(str1, "Languages\\EG1018.FNT");
    makefilename(str2, "Languages\\CG1818.FNT");

    Cute18.LoadFont(FONT_SET_SBCS, str1, 10, 18, FONT_FMT_1, 1);
    Cute18.LoadFont(FONT_SET_DBCS, str2, 18, 18, FONT_FMT_1, 1);

    // Adjust fonts setting
    Ming12.SetDropShadow(RGB_DARKBLACK, 1);
    Ming14.SetDropShadow(RGB_DARKBLACK, 1);
    Ming16.SetDropShadow(RGB_DARKBLACK, 1);
    Cute18.SetDropShadow(RGB_DARKBLACK, 1);

    Ming12.SetSpace(0, 0);
    Ming14.SetSpace(1, 0);
    Ming16.SetSpace(1, 0);
    Cute18.SetSpace(1, 4);

    return TRUE;
}



Ming12.SetColor(RGB_WHITE, 0);
Ming12.PrintfXY(X, Y, "%d", Player[0].GetMonster()->wLevel / 10);
Ming12.PutcXY  (X, Y, 'L');
Ming12.PutsXY  (X, Y, "test ");


