keys[PAGER][CONTROL('C')] = a_quit;
keys[PAGER]['q']          = a_quit;

keys[PAGER][CONTROL('L')] = a_redraw;
keys[PAGER]['r']          = a_redraw;

keys[PAGER]['g']          = a_jump_begin;

keys[PAGER]['G']          = a_jump_end;

keys[PAGER]['b']          = a_page_up;
keys[PAGER][CONTROL('B')] = a_page_up;

keys[PAGER]['f']          = a_page_down;
keys[PAGER][CONTROL('F')] = a_page_down;
keys[PAGER][' ']          = a_page_down;

keys[PAGER]['u']          = a_half_page_up;
keys[PAGER][CONTROL('U')] = a_half_page_up;

keys[PAGER]['d']          = a_half_page_down;
keys[PAGER][CONTROL('D')] = a_half_page_down;

keys[PAGER]['k']          = a_scroll_up;

keys[PAGER]['j']          = a_scroll_down;
keys[PAGER][CONTROL('J')] = a_scroll_down;

keys[PAGER]['v']          = a_editor;
