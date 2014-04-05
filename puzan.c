char *
str_drop_str(char *str, char *sub)
{
	char *p_dst = str;
	char *p_src = str;
	char src, subc;

	char *p_sub = sub;
	int sub_len = 0;

	int delim = 1;

	do {
		src = *p_src;
		subc = *p_sub;

		if (!subc && (src == ' ')) {
			p_dst -= sub_len;
			p_sub = sub;
			sub_len = 0;
		} else if (delim && (src == subc)) {
			p_sub++;
			sub_len++;
		} else {
			delim = (src == ' ') ? 1 : 0;
			p_sub = sub;
			sub_len = 0;
		}

		if (p_dst != p_src)
			*p_dst = src;

		p_src++;
		p_dst++;

	} while (src);

	if (!subc)
		p_dst -= sub_len;

	*p_dst = 0;

	return str;
}

char*
str_mask_str(char* str, const char* sub)
{
    char ch, subc;
    const char* sub_slide = sub;
    char* str_slide = str;
    char* str_fill;

    goto l1;

    while ((ch = *str_slide)) {
        str_slide ++;
        if (ch == ' ') {
        l1:
            for (;;) {
                ch = *str_slide;
                subc = *sub_slide;
                
                if ((subc == ch) && subc) {
                    str_slide ++;
                    sub_slide ++;
                } else
                    break;
            }
    
            if (!subc && (ch == ' ' || ch == '\0')) {
                for (str_fill = str_slide - (sub_slide - sub);
                     str_fill != str_slide; *str_fill++ = ' ');
            }
            
            sub_slide = sub;
        }
    }

    return str;
}
