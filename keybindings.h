void (*k_pager[128])(Arg *);


/* keybindings for PAGER_MODE */

void
keybindings() {
	k_pager[CONTROL('L')] = a_redraw;
	k_pager[CONTROL('c')] = a_quit;
	k_pager['q']          = a_quit;
	k_pager['g']          = a_jump_begin;
	k_pager['G']          = a_jump_end;
	k_pager['u']          = a_half_page_up;
	k_pager['d']          = a_half_page_down;
	k_pager['f']          = a_page_up;
	k_pager['b']          = a_page_down;
	k_pager['k']          = a_scroll_up;
	k_pager['j']          = a_scroll_down;
}
