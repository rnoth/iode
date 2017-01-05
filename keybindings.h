struct keybinding keybindings[] = {
	{ { CONTROL('C') }, a_quit           },
	{ { 'q'          }, a_quit           },

	{ { CONTROL('L') }, a_redraw         },
	{ { 'r'          }, a_redraw         },

	{ { 'g'          }, a_jump_begin     },

	{ { 'G'          }, a_jump_end       },

	{ { 'b'          }, a_page_up        },
	{ { CONTROL('B') }, a_page_up        },

	{ { 'f'          }, a_page_down      },
	{ { CONTROL('F') }, a_page_down      },
	{ { ' '          }, a_page_down      },

	{ { 'u'          }, a_half_page_up   },
	{ { CONTROL('U') }, a_half_page_up   },

	{ { 'd'          }, a_half_page_down },
	{ { CONTROL('D') }, a_half_page_down },

	{ { CONTROL('Y') }, a_scroll_up      },

	{ { CONTROL('J') }, a_scroll_down    },
	{ { CONTROL('E') }, a_scroll_down    },

	{ { 'k'          }, a_cursor_up      },
	{ { 'j'          }, a_cursor_down    },

	{ { 'v'          }, a_editor         },
};
