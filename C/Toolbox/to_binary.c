int to_binary(unsigned long long a, size_t bytes, char * string)
{
    // Description
    //
    // A tool to get a string representing
    // the binary value of the argument a,
    // useful for debugging.
    //
    // There are certainly more robust
    // ways of doing this, here's mine
    //
    // D.Dugas

    size_t bits = bytes*8;

    size_t string_length = bits + bytes; //1 extra space or null char per byte

#define FIRST_BIT (0x01ULL)

    size_t i;
    size_t s = bits; //shift count
    for(i=0;i<string_length;i++)
    {
        if ((i+1) % 9)
        {
            s--;
            string[i] = (char)'0'+( (a & (FIRST_BIT << s)) > 0);
        }
        else
        {
            string[i] = ' '; //insert a space after every byte
        }
    }
    string[string_length-1]=0; //ensure null-terminated string


    return 0;
}
