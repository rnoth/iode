	for (n_total = 0; !feof(file) && !ferror(file); n_total++) {
		char str[MAX_LINE_SIZE];
		size_t i = 0;

		/* read a new line accepting null characters */
		while (i < MAX_LINE_SIZE && (str[i] = fgetc(file)) != EOF) {
			if (str[i] == '\n')
				break;
			i++;
		}

		/* create a new line and append it to the buffer */
		l_current = new_line(str, i);
		link_lines(l_last, l_current);
		l_last = l_current;
		l_first = l_first ? l_first : l_current;
	}
